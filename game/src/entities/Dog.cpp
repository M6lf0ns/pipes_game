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
        {1,1,1,1},
        physicsWorld,
        EntityType::Dog) {

        this->sprite = new Sprite(position, scale, Texture::LoadTexture("assets/textures/test_secBot.png"));
        Dog::createPhysicsBody();

    }

    void Dog::update(Game *game, float deltaTime) {

        // In Dog::update
        b2Vec2 dogPos = b2Body_GetPosition(body);
        std::cout << "Dog Physical Left Edge: " << (dogPos.x - dogRadius) << std::endl;
        Entity::update(game, deltaTime);
        if (this->stickHasLanded) {
            b2Vec2 currentVel = b2Body_GetLinearVelocity(body);
            float distance = std::abs(targetLocation - position.x);
            float stopThreshold = 0.05f; // Adjust this based on your scale
            float direction = (targetLocation > position.x) ? 1.0f : -1.0f;
            std::cout << "DOG: ACTUALLY MOVING NOW" << std::endl;
            if (distance < stopThreshold) {
                b2Body_SetLinearVelocity(body, b2Vec2_zero);
            }
            else {
                b2Body_SetLinearVelocity(body,{static_cast<float>(0.5f * direction), currentVel.y});
            }


            // 3. Force it to stay awake
            b2Body_SetAwake(body, true);

        }


    }




    void Dog::createPhysicsBody() {

        setDogRadius(scale.y /2.0f);
        b2BodyDef bodyDef = b2DefaultBodyDef();
        bodyDef.enableSleep = true;
        bodyDef.type = b2_dynamicBody;
        bodyDef.position = {position.x, position.y};
        bodyDef.userData = this;
        body = b2CreateBody(physicsWorld, &bodyDef);

        b2Circle circleGeometry = { {0.0f, 0.0f}, dogRadius };

        // 4. Define the Shape Properties
        b2ShapeDef dogShapeDef = b2DefaultShapeDef();
        dogShapeDef.filter.categoryBits = gl3::PhysicsCategories::CATEGORY_DOG;
        dogShapeDef.filter.maskBits = gl3::PhysicsCategories::CATEGORY_FLOOR |
                                  gl3::PhysicsCategories::CATEGORY_WALL |
                                  gl3::PhysicsCategories::CATEGORY_BOX |
                                      gl3::PhysicsCategories::CATEGORY_DOOR;


        dogShapeDef.userData = this;
        shape = b2CreateCircleShape(
            body,
            &dogShapeDef,
            &circleGeometry
            );
        b2Shape_SetFriction(shape, 0.3f);
        b2Shape_SetRestitution(shape, 0.1f);


        b2MassData massData = b2Body_GetMassData(body);
        massData.rotationalInertia = 0.0f;
        b2Body_SetMassData(body, massData);
        b2Body_SetAwake(body, true); // Force the dog to start moving immediately
    }


    void Dog::startContact() {

    }




}
