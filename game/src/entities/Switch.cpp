
#include "Switch.h"
#include "../PhysicsCategories.h"
#include <iostream>
#include "../rendering/Texture.h"
#include "box2d/box2d.h"

namespace gl3 {
    Switch::Switch(Shader* shader, glm::vec3 position, glm::vec3 scale, int floorid, b2WorldId physicsWorld)
        :Entity(shader,
            position,
            scale,
            physicsWorld,
              EntityType::Switch),
    floorID(floorid)

    {
        this->sprite = new Sprite(position, scale, Texture::LoadTexture("assets/textures/switch.png"));
        this->setZOrder(1);
        //assuming scale.x and scale.y already halfheight and -width
        halfHeight = scale.y / 2.0f;
        halfWidth = scale.x / 2.0f;

        Switch::createPhysicsBody();
        //Align sprite center with physicsbody
        //sprite.setPosition(glm::vec3(position.x,position.y,0.0f));
        //sprite.setScale(glm::vec3(halfWidth * 2, halfHeight * 2, 1.0f)); //full width and height
    }


    void Switch::createPhysicsBody() {
        // 1. Create the Body (The "Anchor" for the logic)
        b2BodyDef bodyDef = b2DefaultBodyDef();
        bodyDef.type = b2_staticBody; // It stays in one spot
        bodyDef.position = {position.x, position.y};
        bodyDef.userData = this;
        body = b2CreateBody(physicsWorld, &bodyDef);

        // 2. Create the Shape (The "Ghost" Trigger)
        // Make it narrow so the triangle-ship tip has to be centered.
        // Make it thin so the box is definitely "on" it before it fires.
        float sensorWidth = 0.05f; // 30% of the sprite width
        float sensorHeight = 0.05f;           // Very thin
        b2Polygon sensorBox = b2MakeBox(sensorWidth, sensorHeight);

        // 3. Configure as a SENSOR
        b2ShapeDef shapeDef = b2DefaultShapeDef();
        shapeDef.isSensor = true; // <--- THIS is what makes it a "Ghost"
        shapeDef.filter.categoryBits = gl3::PhysicsCategories::CATEGORY_SWITCH;
        shapeDef.filter.maskBits = gl3::PhysicsCategories::CATEGORY_PLAYER |
                                   gl3::PhysicsCategories::CATEGORY_BOX |
                                       gl3::PhysicsCategories::CATEGORY_DOG |
                                           gl3::PhysicsCategories::CATEGORY_SEC_BOT |
                                               gl3::PhysicsCategories::CATEGORY_STICK;

        // Attach it
        shape = b2CreatePolygonShape(body, &shapeDef, &sensorBox);
        b2Shape_SetUserData(shape, this);
        b2Shape_EnableSensorEvents(shape, true);
    }


    float Switch::getHalfHeight() const {
        return halfHeight;
    }

    float Switch::getHalfWidth() const {
        return halfWidth;
    }

    void Switch::onSensorEnter(Entity* visitor) {
        if (!visitor) return;

        // Log the pointer and the type
        //std::cout << "[S-ENTER] Ptr: " << visitor << " Type: " << (int)visitor->getType() << std::endl;

        if (visitor->getType() == EntityType::Box || visitor->getType() == EntityType::Player || visitor->getType() == EntityType::Dog || visitor->getType() == EntityType::SecBot) {
            occupants.insert(visitor);
            //std::cout << "[S-COUNT] Now holding: " << occupants.size() << " objects." << std::endl;
            if (targetDoor) targetDoor->openDoor();
        }
        else if (visitor->getType() == EntityType::Dog) {
            //std::cout << "DOG HAS ENTERED SWITCH SENSOR" << std::endl;
        }
        else if(visitor ->getType() == EntityType::Stick) {
            sticko = dynamic_cast<Stick*>(visitor);
            sticko->setFloorID(this->getFloorID());
            std::cout << "SWITCH FLOOR ID: " << this->getFloorID() << std::endl;
            std::cout << "Stick ID: " << sticko->getFloorID() << std::endl;
        }
    }

    void Switch::onSensorExit(Entity* visitor) {
        if (!visitor) return;

        //std::cout << "[S-EXIT] Ptr: " << visitor << std::endl;

        auto it = occupants.find(visitor);
        if (it != occupants.end()) {
            occupants.erase(it);
            //std::cout << "[S-COUNT] Now holding: " << occupants.size() << " objects." << std::endl;
        } else {
            //std::cout << "[S-WARNING] An object left that was never registered!" << std::endl;
        }

        if (occupants.empty() && targetDoor) {
            std::cout << "[S-LOGIC] Set empty. Closing Door." << std::endl;
            targetDoor->closeDoor();
        }
    }



}
