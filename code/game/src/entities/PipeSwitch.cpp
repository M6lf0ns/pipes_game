#include "PipeSwitch.h"
#include "../rendering/Texture.h"
#include "box2d/box2d.h"
#include <iostream>
#include "../Game.h"

#include "Ship.h"
#include "../PhysicsCategories.h"

namespace gl3 {
    PipeSwitch::PipeSwitch(Shader *shader, glm::vec3 position, glm::vec3 scale, b2WorldId physicsWorld)
        :Entity( shader,
            position,
            scale,
            physicsWorld,
            EntityType::PipeSwitch)
    {
        this->sprite = new Sprite(position, scale, Texture::LoadTexture("assets/textures/switch_off.png"));
        this->setZOrder(1);

        PipeSwitch::createPhysicsBody();

        sprite->setPosition(glm::vec3(position.x, position.y, 0.0f));
        sprite->setScale(glm::vec3(scale.x, scale.y, 1.0f));

    }

    void PipeSwitch::createPhysicsBody() {
        b2BodyDef bodyDef = b2DefaultBodyDef();
        bodyDef.type = b2_staticBody;
        bodyDef.position = {position.x, position.y};
        bodyDef.userData = this;
        body = b2CreateBody(physicsWorld, &bodyDef);

        b2Polygon switchBox = b2MakeBox(scale.x/2.0f, scale.y / 2.0f);

        b2ShapeDef shapeDef = b2DefaultShapeDef();
        shapeDef.isSensor = true;
        shapeDef.filter.categoryBits = gl3::PhysicsCategories::CATEGORY_SPEAKER_SWITCH;
        shapeDef.filter.maskBits = gl3::PhysicsCategories::CATEGORY_PLAYER;
        shapeDef.userData = this;

        shape = b2CreatePolygonShape(body, &shapeDef, &switchBox);
        b2Shape_SetUserData(shape, this);
        b2Shape_EnableSensorEvents(shape, true);
    }

    void PipeSwitch::update(Game *game, float deltaTime) {
        Entity::update(game, deltaTime);


    }


    void PipeSwitch::onSensorEnter(Entity *other) {
        //std::cout << "[DEBUG] Sensor overlap detected!" << std::endl; // Add this!
    if (other && other->getType() == EntityType::Player) {
        Ship* player = dynamic_cast<Ship*>(other);
        canBeFlipped = true;
        player->setCurrentPipeSwitch(this);
        std::cout << "We have set the player for the pipeswitch" << std::endl;
        //std::cout << canBeFlipped << " CURRENT SWITCH SET" << std::endl;

    }
    }

    void PipeSwitch::onSensorExit(Entity *other) {
        if (other && other->getType() == EntityType::Player) {
            Ship* player = dynamic_cast<Ship*>(other);
            canBeFlipped = false;
            if (player) { // Extra safety
                player->setCurrentPipeSwitch(nullptr);

            }

        }

    }


    void PipeSwitch::flipSwitch(Game* game) {
        if (!targetCube) return;
        isOn = !isOn;
        float nextAngle = std::fmod(targetCube->getPipeRotation() + 45.0f, 360.0f);
        targetCube->rotatePipe(nextAngle, true);
        game->PlaySfx(game->getFlipSwitchSFX(), 0.3f);
    }
    }



