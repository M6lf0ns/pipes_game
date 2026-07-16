#include "Dog.h"
#include <iostream>
#include "../Game.h"
#include "../rendering/Texture.h"
#include "box2d/box2d.h"
#include "../PhysicsCategories.h"

namespace gl3 {

    float gl3::Dog::dogRadius;
    Dog::Dog(Shader *shader, glm::vec3 position, glm::vec3 scale, int floorid, b2WorldId physicsWorld) : Entity(
        shader,
        position,
        scale,
        physicsWorld,
        EntityType::Dog),
    floorID(floorid)
    {

        this->sprite = new Sprite(position, scale, Texture::LoadTexture("assets/textures/hog_idle.png"));
        this->setZOrder(3);
        Dog::createPhysicsBody();

        idleTexture = Texture::LoadTexture("assets/textures/hog_idle.png");


        this->walkingFrames.push_back(Texture::LoadTexture("assets/textures/hog_walk_1.png"));
        this->walkingFrames.push_back(Texture::LoadTexture("assets/textures/hog_walk_2.png"));
        this->walkingFrames.push_back(Texture::LoadTexture("assets/textures/hog_walk_3.png"));
        this->walkingFrames.push_back(Texture::LoadTexture("assets/textures/hog_walk_4.png"));
        this->walkingFrames.push_back(Texture::LoadTexture("assets/textures/hog_walk_5.png"));

    }

    void Dog::draw(Game *game) {
        if (!isVisible) return;
        Entity::draw(game);
    }


    void Dog::update(Game *game, float deltaTime) {
        // At the very top of Dog::update
        if (enVent != nullptr && !b2Body_IsValid(enVent->getBody())) {
            // If the vent was deleted by the Scene, drop the pointer immediately!
            enVent = nullptr;
            exVent = nullptr;
        }
        if (enVent != nullptr) {
            // If the pointer exists, verify it's still a valid memory object
            // For example, check if its Type is still correct
            if (enVent->getType() != EntityType::Vent) {
                std::cout << "[CRITICAL ERROR] enVent pointer is corrupted!" << std::endl;
                enVent = nullptr; // Reset to clean state
            }
        }

        SoLoud::handle h;
        if (isWalking) {
            animTimer += deltaTime;
            if (animTimer >= 0.05f) {
                // Your frame speed
                animTimer = 0.0f;
                animFrame = (animFrame + 1) % walkingFrames.size();
                if (animFrame == 0 || animFrame == 2) {
                    game->getAudio().stop(walkSoundHandle); // stop previous
                    walkSoundHandle = game->PlaySfxWithHandle(game->getDogWalkSFX());
                    game->getAudio().setRelativePlaySpeed(walkSoundHandle, 1.5f);
                }
                // HERE is the assignment!
                this->sprite->setTexture(walkingFrames[animFrame]);
                Entity::update(game, deltaTime);
            }
        } else {
            this->sprite->setTexture(idleTexture);
            game->getAudio().stop(walkSoundHandle);
            Entity::update(game, deltaTime);
        }


    if (this->stickHasLanded) {



        float stopThreshold = 0.06f;

        onWrongFloor = this->getFloorID() != targetFloorID;
        // 1. ALWAYS CHECK TELEPORTATION FIRST

        if (onWrongFloor && enVent != nullptr && enVent->getIsOpen() && stickInVent) {
            targetLocation = enVent->getPosition();
        }



        if (enVent != nullptr && exVent != nullptr && enVent->getIsOpen() && !stickWasThrown) {



            std::cout << "WE ARE REACHING TELEPORTATION CODE" << std::endl;
            float distance = glm::distance(enVent->getPosition().x, this->getPosition().x);
            float maxAllowedDist = 0.07f;

            if (distance <= maxAllowedDist) {
                glm::vec3 exVentPos = exVent->getPosition();

                // Teleport physical body and rendering position
                b2Body_SetTransform(body, {exVentPos.x, exVentPos.y}, b2MakeRot(0.0f));
                this->position = exVentPos;

                this->stickWasThrown = false; // Reset this so the dog stops "thinking" about the throw
                setTargetLocation(this->getPosition());
                b2Body_SetLinearVelocity(body, b2Vec2_zero);


                isWalking = false;
                // Shut down state variables cleanly
                this->setDogHasTeleported(true);
                this->stickHasLanded = false;
                this->stickInVent = false;


                return; // Exit completely so no movement calculations happen!
            }

        }

        if (enVent != nullptr && targetLocation == enVent->getPosition()) {
            std::cout << "ENVENT AND TARGETLOCATION EQUAL " <<  std::endl;

        }
        else {
            std:: cout << "TARGETLOCATION IS NOT ENVENTS LOCATION " << std::endl;
        }

        std::cout << "DOG LOCATION " << this->position.x << " " << this->position.y << std::endl;
        std::cout << "TARGET LOCATION " << targetLocation.x << " " << targetLocation.y << std::endl;
        // 2. MOVEMENT CALCULATIONS ONLY RUN IF WE DID NOT TELEPORT
        b2Vec2 currentVel = b2Body_GetLinearVelocity(body);
        float dist = std::abs(targetLocation.x - this->position.x);

        float direction = (targetLocation.x  > position.x) ? 1.0f : -1.0f;

        if (dist > stopThreshold) {
            // Only update direction if we aren't "vibrating" at an edge
            if (std::abs(currentVel.x) > 0.05f) {
                direction = (targetLocation.x > position.x) ? 1.0f : -1.0f;
            }

            // Apply the locked direction
            b2Body_SetLinearVelocity(body, {static_cast<float>(0.6f * direction), currentVel.y});

            // Flip sprite based on the locked direction
            float scaleX = std::abs(sprite->getScale().x);
            sprite->setScale(glm::vec3(direction > 0 ? scaleX : -scaleX, sprite->getScale().y, sprite->getScale().z));
            isWalking = true;
        } else {
            // We are at the target
            b2Body_SetLinearVelocity(body, {0.0f, currentVel.y});
            isWalking = false;
        }

        b2Body_SetAwake(body, true);
    }
}



    void Dog::createPhysicsBody() {

        // 1. Calculate radius locally first to ensure it's valid
        float radius = scale.x / 3.5f;
        //if (radius <= 0.0f) radius = 0.5f; // Emergency fallback

        b2BodyDef bodyDef = b2DefaultBodyDef();
        bodyDef.type = b2_dynamicBody;
        bodyDef.position = {position.x, position.y};
        bodyDef.userData = this;
        bodyDef.enableSleep = false; // Add this line!
        body = b2CreateBody(physicsWorld, &bodyDef);

        // 2. Pass the radius directly
        b2Circle circleGeometry = { {0.0f, 0.0f}, radius };

        b2ShapeDef dogShapeDef = b2DefaultShapeDef();
        dogShapeDef.filter.categoryBits = gl3::PhysicsCategories::CATEGORY_DOG;
        dogShapeDef.filter.maskBits = gl3::PhysicsCategories::CATEGORY_FLOOR |
                                  gl3::PhysicsCategories::CATEGORY_WALL |
                                  gl3::PhysicsCategories::CATEGORY_BOX |
                                      gl3::PhysicsCategories::CATEGORY_DOOR |
                                          gl3::PhysicsCategories::CATEGORY_SWITCH |
                                              gl3::PhysicsCategories::CATEGORY_VENT;


        dogShapeDef.userData = this;
        shape = b2CreateCircleShape(
            body,
            &dogShapeDef,
            &circleGeometry
            );
        std::cout << "[DOG DEBUG] Created with Radius: " << radius << " at Pos: " << position.x << ", " << position.y << std::endl;
        b2Shape_SetFriction(shape, 0.3f);
        b2Shape_SetRestitution(shape, 0.1f);
        // This is crucial for sensors to "see" this dynamic body
        b2Shape_EnableSensorEvents(shape, true);


        b2MassData massData = b2Body_GetMassData(body);
        massData.rotationalInertia = 0.0f;
        b2Body_SetMassData(body, massData);
        b2Body_SetAwake(body, true); // Force the dog to start moving immediately

    }


    void Dog::startContact(Entity* other) {
        if (other && other->getType() == EntityType::Floor) {
            floor = dynamic_cast<Floor*>(other);
            this->setFloorID(floor->getFloorID());
        }


    }




}
