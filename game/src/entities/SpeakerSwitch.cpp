#include "SpeakerSwitch.h"
#include "../rendering/Texture.h"
#include "box2d/box2d.h"
#include <iostream>

#include "Ship.h"
#include "../PhysicsCategories.h"

namespace gl3 {
    SpeakerSwitch::SpeakerSwitch(Shader *shader, glm::vec3 position, glm::vec3 scale, b2WorldId physicsWorld)
        :Entity( shader,
            position,
            scale,
            physicsWorld,
            EntityType::SpeakerSwitch)
    {
        this->sprite = new Sprite(position, scale, Texture::LoadTexture("assets/textures/switch_off.png"));
        this->setZOrder(1);

        SpeakerSwitch::createPhysicsBody();

        sprite->setPosition(glm::vec3(position.x, position.y, 0.0f));
        sprite->setScale(glm::vec3(scale.x, scale.y, 1.0f));

    }

    void SpeakerSwitch::createPhysicsBody() {
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

    void SpeakerSwitch::onSensorEnter(Entity *other) {
        //std::cout << "[DEBUG] Sensor overlap detected!" << std::endl; // Add this!
    if (other && other->getType() == EntityType::Player) {
        Ship* player = dynamic_cast<Ship*>(other);
        canBeFlipped = true;
        player->setCurrentSpeakerSwitch(this);
        //std::cout << canBeFlipped << " CURRENT SWITCH SET" << std::endl;

    }
    }

    void SpeakerSwitch::onSensorExit(Entity *other) {
        if (other && other->getType() == EntityType::Player) {
            Ship* player = dynamic_cast<Ship*>(other);
            canBeFlipped = false;
            if (player) { // Extra safety
                player->setCurrentSpeakerSwitch(nullptr);

            }

        }

    }

    void SpeakerSwitch::flipSwitch() {
        if (!targetSpeaker) {
            std::cout << "CRITICAL ERROR: SpeakerSwitch has NO targetSpeaker linked!" << std::endl;
            return;
        }
        isOn = !isOn;
        std::cout << "Switch is now: " << (isOn ? "ON" : "OFF") << std::endl;

        if (isOn) targetSpeaker->StartNoise();
        else targetSpeaker->CeaseNoise();
    }



}