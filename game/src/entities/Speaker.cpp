#include "Speaker.h"
#include "../rendering/Texture.h"
#include "box2d/box2d.h"
#include "box2d/types.h"
#include <iostream>
#include "../PhysicsCategories.h"
#include "SecBot.h"
#include "../Game.h"

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
        speakerOffTexture = Texture::LoadTexture("assets/textures/Speaker.png");
        speakerOnFrames.push_back(Texture::LoadTexture("assets/textures/Speaker-1.png"));
        speakerOnFrames.push_back(Texture::LoadTexture("assets/textures/Speaker-2.png"));
        speakerOnFrames.push_back(Texture::LoadTexture("assets/textures/Speaker-3.png"));
        speakerOnFrames.push_back(Texture::LoadTexture("assets/textures/Speaker-4.png"));
        speakerOnFrames.push_back(Texture::LoadTexture("assets/textures/Speaker-5.png"));
        Speaker::createPhysicsBody();

        sprite->setPosition(glm::vec3(position.x, position.y, 0.0f));
        sprite->setScale(glm::vec3(scale.x, scale.y, 1.0f));
    }

    void Speaker::update(Game* game, float dt) {
        // If the switch is ON and we have a bot in our radius,
        // keep it frozen to be safe.

        Entity::update(game, dt);
        if (isActive) {
            soundTimer -= dt; // Count down

            if (soundTimer <= 0.0f) {
                game->PlaySfx(game->getBotStunSFX());
                soundTimer = SOUND_INTERVAL; // Reset the timer
            }
        } else {
            // Reset timer if inactive, so it plays immediately when it turns on again
            soundTimer = 0.0f;
        }


        animTimer += dt;
        if (animTimer >= 0.05f) { // Your frame speed
            animTimer = 0.0f;
        if (isActive) {
            animFrame = (animFrame + 1) % speakerOnFrames.size();
            this->sprite->setTexture(speakerOnFrames[animFrame]);
            this->sprite->setScale(glm::vec3(scale.x +0.05f, scale.y, 1.0f));


        }
        else {
                this->sprite->setTexture(speakerOffTexture);
            this->sprite->setScale(glm::vec3(scale.x, scale.y, 1.0f));
            }





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
        b2Polygon sensorBox = b2MakeBox(scale.x/1.2f, scale.y/2.0f);

        b2ShapeDef shapeDef = b2DefaultShapeDef();
        shapeDef.isSensor = true;
        shapeDef.filter.categoryBits = gl3::PhysicsCategories::CATEGORY_SPEAKER;
        shapeDef.filter.maskBits = gl3::PhysicsCategories::CATEGORY_PLAYER |
            gl3::PhysicsCategories::CATEGORY_SEC_BOT;
        shapeDef.userData = this;

        shape = b2CreatePolygonShape(body, &shapeDef, &sensorBox);
        b2Shape_SetUserData(shape, this);
        b2Shape_EnableSensorEvents(shape, true);

        // 2. NEW solid shape just for raycast detection
        b2Polygon rayBox = b2MakeBox(scale.x/4.0f, scale.y/2.0f);
        b2ShapeDef rayDef = b2DefaultShapeDef();
        rayDef.isSensor = false; // solid so raycasts hit it
        rayDef.filter.categoryBits = gl3::PhysicsCategories::CATEGORY_SPEAKER;
        rayDef.filter.maskBits = gl3::PhysicsCategories::CATEGORY_RAY_SENSOR;
        rayDef.userData = this;
        b2ShapeId rayShape = b2CreatePolygonShape(body, &rayDef, &rayBox);
        b2Shape_SetUserData(rayShape, this);
        isActive = false;
    }

    void Speaker::onSensorEnter(Entity* other) {
        if (other && other->getType() == EntityType::SecBot) {
            this->currentTarget = static_cast<SecBot*>(other);
            botEnteredWhileActive = isActive; // track if speaker was on when bot entered
        }
    }

    void Speaker::onSensorExit(Entity* other) {
        if (other == currentTarget) {
            setCurrentTarget(nullptr);
        }
    }

    void Speaker::StartNoise() {
        this->isActive = true;
        if (currentTarget && !botEnteredWhileActive) {
            currentTarget->Freeze(); // bot was inside before activation, freeze it
        }
    }
    void Speaker::CeaseNoise() {
        this->isActive = false;
        if (currentTarget) {
            currentTarget->UnFreeze();
            // KEIN manueller Velocity-Schub hier!
            // Lass die normale KI-Logik des Bots übernehmen.
        }
    }

}









