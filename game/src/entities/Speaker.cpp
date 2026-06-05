#include "Speaker.h"
#include "../rendering/Texture.h"
#include "box2d/box2d.h"
#include "box2d/types.h"
#include <iostream>
#include "../PhysicsCategories.h"
#include "SecBot.h"

namespace gl3 {
    Speaker::Speaker(Shader *shader, glm::vec3 position, glm::vec3 scale, b2WorldId physicsWorld)
        :Entity(shader,
            position,
            scale,
            physicsWorld,
            EntityType::Speaker)
    {
        this->sprite = new Sprite(position, scale, Texture::LoadTexture("assets/textures/Speaker.png"));
        this->setZOrder(0);

        Speaker::createPhysicsBody();

        sprite->setPosition(glm::vec3(position.x, position.y, 0.0f));
        sprite->setScale(glm::vec3(scale.x, scale.y, 1.0f));
    }

    void Speaker::update(Game* game, float dt) {
        // If the switch is ON and we have a bot in our radius,
        // keep it frozen to be safe.
        if (isActive && currentTarget) {
            currentTarget->Freeze();
        }
    }

    void Speaker::createPhysicsBody() {
        b2BodyDef bodyDef = b2DefaultBodyDef();
        bodyDef.type = b2_staticBody;
        bodyDef.position = {position.x, position.y};
        bodyDef.userData = this;
        body = b2CreateBody(physicsWorld, &bodyDef);

        // Make the sensor height exactly the same as the ladder.
        // If the floor is 0.1 thick and sits exactly at the ladder's top,
        // the player's feet will still be "touching" the top edge of the sensor.
        b2Polygon sensorBox = b2MakeBox(scale.x/2.0f, scale.y/2.0f);

        b2ShapeDef shapeDef = b2DefaultShapeDef();
        shapeDef.isSensor = true;
        shapeDef.filter.categoryBits = gl3::PhysicsCategories::CATEGORY_SPEAKER;
        shapeDef.filter.maskBits = gl3::PhysicsCategories::CATEGORY_PLAYER |
                                    gl3::PhysicsCategories::CATEGORY_SEC_BOT;
        shapeDef.userData = this;

        shape = b2CreatePolygonShape(body, &shapeDef, &sensorBox);
        b2Shape_SetUserData(shape, this);
        b2Shape_EnableSensorEvents(shape, true);
        isActive = false;
    }

    void Speaker::onSensorEnter(Entity* other) {
        if (other && other->getType() == EntityType::SecBot) {
            //std::cout << "SECBOT IS BEING DETECTED BY SPEAKER" << std::endl;
            this->currentTarget = static_cast<SecBot*>(other);
            //std::cout << "Speaker has LOCKED ON to SecBot" << std::endl;

            // If the switch was ALREADY on when the bot walked in, freeze it immediately
            if (isActive) {
                currentTarget->Freeze();
            }
        }
    }

    void Speaker::onSensorExit(Entity* other) {
        if (other == currentTarget) {
            setCurrentTarget(nullptr);
        }
    }

    void Speaker::StartNoise() {
        this->isActive = true; // Track the "ON" state of the speaker
        if (currentTarget) {
            currentTarget->Freeze();
        }
    }

    void Speaker::CeaseNoise() {
        this->isActive = false;
        if (currentTarget) {
            //std::cout << "Releasing bot from speaker hold." << std::endl;
            currentTarget->UnFreeze();
            // Crucial: Manually apply a small velocity so it
            // actually exits the sensor area.
            b2Body_SetLinearVelocity(currentTarget->getBody(), {2.0f, 0.0f});
        }
    }

}









