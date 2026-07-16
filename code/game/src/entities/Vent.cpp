#include "Vent.h"
#include "Stick.h"
#include "Dog.h"
#include <iostream>
#include "Ship.h"
#include "../rendering/Texture.h"
#include "box2d/box2d.h"
#include "box2d/types.h"
#include "../PhysicsCategories.h"

namespace gl3 {
    Vent::Vent(Shader *shader, glm::vec3 position, glm::vec3 scale, b2WorldId physicsWorld, int linkID, bool isExit) :
    Entity( shader,
        position,
        scale,
        physicsWorld,
        EntityType::Vent
        )
    {
        this->ventId = linkID;
        this->connectedTwin = nullptr;
        this->isExit= isExit; // Directly set state from file (true for exit, false for entrance)
        this->ventIsOpen = isExit;
        if (this->ventIsOpen) {
            this->sprite = new Sprite(position, scale, Texture::LoadTexture("assets/textures/Vent_opened.png"));
        } else {
            this->sprite = new Sprite(position, scale, Texture::LoadTexture("assets/textures/Vent_closed.png"));
        }
        this->setZOrder(0);

        Vent::createPhysicsBody();

        sprite->setPosition(glm::vec3(position.x, position.y, 0.0f));
        sprite->setScale(glm::vec3(scale.x, scale.y, 1.0f));
    }

    void Vent::createPhysicsBody() {
        b2BodyDef bodyDef = b2DefaultBodyDef();
        bodyDef.type = b2_staticBody;
        bodyDef.position = {position.x, position.y};
        bodyDef.userData = this;
        body = b2CreateBody(physicsWorld, &bodyDef);

        // Make the sensor height exactly the same as the ladder.
        // If the floor is 0.1 thick and sits exactly at the ladder's top,
        // the player's feet will still be "touching" the top edge of the sensor.
        b2Polygon sensorBox = b2MakeBox(scale.x/2.0f, scale.y* 5.0f);

        b2ShapeDef shapeDef = b2DefaultShapeDef();
        shapeDef.isSensor = true;
        shapeDef.filter.categoryBits = gl3::PhysicsCategories::CATEGORY_VENT;
        shapeDef.filter.maskBits = gl3::PhysicsCategories::CATEGORY_PLAYER |
                                    gl3::PhysicsCategories::CATEGORY_DOG;
        shapeDef.userData = this;

        shape = b2CreatePolygonShape(body, &shapeDef, &sensorBox);
        b2Shape_SetUserData(shape, this);
        b2Shape_EnableSensorEvents(shape, true);
    }

    void Vent::onSensorEnter(Entity *other) {
        if (other) {
            if (other->getType() == EntityType::Player) {
                Ship* player = dynamic_cast<Ship*>(other);
                player->setCorrespVent(this);
                std::cout <<  " VENT DETECTED PLAYER" << std::endl;
            }
            else if (other->getType() == EntityType::Dog) {
                Dog* dog = dynamic_cast<Dog*>(other);
                if (!(this->isExit)) {
                    dog->setExitVent(this->getConnectedTwin());
                    dog->setEntryVent(this);
                    std::cout << "ENTRY VENT"   << dog->getEntryVent() << std::endl;
                    std::cout << "EXIT VENT"   << dog->getExitVent() << std::endl;
                }

            }
        }
    }

    void Vent::onSensorExit(Entity *other) {
        if (other && other->getType() == EntityType::Player) {
            Ship* player = dynamic_cast<Ship*>(other);
            // player->setCorrespVent(nullptr);
        }
        else if (other && other->getType() == EntityType::Dog) {
            Dog* dog = dynamic_cast<Dog*>(other);

            // ONLY clear the pointers if the dog is exiting its registered entrance vent naturally.
            // If it teleported, 'this' will match its entrance, but we only clear it if we aren't
            // trying to preserve a tracking state.

            // std::cout << "IS DOGHASTELEPORTED TRUE? " << dog->getDogHasTeleported() << std::endl;
            // if (dog->getEntryVent() == this) {
            //     dog->setEntryVent(nullptr);
            //     dog->setExitVent(nullptr);
            // }
        }

    }

    void Vent::Open() {
        ventIsOpen = true;
        this->sprite = new Sprite(position, scale, Texture::LoadTexture("assets/textures/Vent_opened.png"));

    }

    void Vent::initConnectedTwin(Vent* twin) {
        this->connectedTwin = twin;
        std::cout << "[DEBUG] Vent " << this << " linked to Twin " << twin << std::endl;

        // If our twin is open (like the unreachable exit), we stay synced
        // or use this space to handle your logic initialization!
        if (this->connectedTwin->getIsOpen()) {
            // Add any specific visual initialization here if needed
        }
    }



}