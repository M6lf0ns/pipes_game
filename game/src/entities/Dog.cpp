#include "Dog.h"
#include <iostream>
#include "../Game.h"
#include "../rendering/Texture.h"
#include "box2d/box2d.h"
#include "../PhysicsCategories.h"

namespace gl3 {

    float gl3::Dog::dogRadius;
    Dog::Dog(Shader *shader, glm::vec3 position, glm::vec3 scale, b2WorldId physicsWorld) : Entity(
        shader,
        position,
        scale,
        physicsWorld,
        EntityType::Dog) {

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
        if (isWalking) {
            animTimer += deltaTime;
            if (animTimer >= 0.05f) { // Your frame speed
                animTimer = 0.0f;
                animFrame = (animFrame + 1) % walkingFrames.size();

                // HERE is the assignment!
                this->sprite->setTexture(walkingFrames[animFrame]);
                Entity::update(game, deltaTime);
            }
        } else {
            this->sprite->setTexture(idleTexture);
            Entity::update(game, deltaTime);
        }


    if (this->stickHasLanded) {

        if (enVent == nullptr) {
            std::cout << "ENTRY VENT NOT RECEIVED" << std::endl;
        }
        if (exVent == nullptr) {
            std::cout << "EX VENT NOT RECEIVED" << std::endl;
        }
        // 1. ALWAYS CHECK TELEPORTATION FIRST
        if (enVent && exVent && enVent->getIsOpen()) {
            std::cout << "WE ARE REACHING TELEPORTATION CODE" << std::endl;
            float distance = glm::distance(enVent->getPosition().x, this->getPosition().x);
            float maxAllowedDist = 0.01f;
            if (distance <= maxAllowedDist) {
                glm::vec3 exVentPos = exVent->getPosition();

                // Teleport physical body and rendering position
                b2Body_SetTransform(body, {exVentPos.x, exVentPos.y}, b2MakeRot(0.0f));
                this->position = exVentPos;

                // Shut down state variables cleanly
                this->stickHasLanded = false;
                b2Body_SetLinearVelocity(body, b2Vec2_zero);

                this->setEntryVent(nullptr);
                this->setExitVent(nullptr);
                isWalking = false;
                return; // Exit completely so no movement calculations happen!
            }
        }

        // 2. MOVEMENT CALCULATIONS ONLY RUN IF WE DID NOT TELEPORT
        b2Vec2 currentVel = b2Body_GetLinearVelocity(body);
        float dist = std::abs(targetLocation - position.x);
        float stopThreshold = 0.05f;
        float direction = (targetLocation > position.x) ? 1.0f : -1.0f;

        // std::cout << "DOG POS: " << position.x
        //           << " | TARGET: " << targetLocation
        //           << " | DIST: " << dist
        //           << " | VEL: " << currentVel.x << std::endl;

        if (dist < stopThreshold) {
            b2Body_SetLinearVelocity(body, b2Vec2_zero);
            isWalking = false;
            // this->sprite = idleSprite;
        }
        else {

            b2Body_SetLinearVelocity(body, {static_cast<float>(0.6f * direction), currentVel.y});
            if (direction > 0.0f) {
                sprite->setScale(glm::vec3(abs(sprite->getScale().x), sprite->getScale().y, sprite->getScale().z));
            }
            else {
                sprite->setScale(glm::vec3(-abs(sprite->getScale().x), sprite->getScale().y, sprite->getScale().z));
            }
            isWalking = true;
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
        if (other) {
            std::cout << "[DOG COLLISION] Hit an entity of type: "
                      << static_cast<int>(other->getType())
                      << " at position X: " << other->getPosition().x << std::endl;
        }
    }




}
