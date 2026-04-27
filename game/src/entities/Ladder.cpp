#include "Ladder.h"
#include "../rendering/Texture.h"
#include "box2d/box2d.h"
#include "box2d/types.h"
#include <iostream>
#include "../PhysicsCategories.h"

namespace gl3 {
    Ladder::Ladder(Shader* shader, glm::vec3 position, glm::vec3 scale, b2WorldId physicsWorld)
      :Entity(shader,
          position,
          scale,
          physicsWorld,
          EntityType::Door)

    {
        this->sprite = new Sprite(position, scale, Texture::LoadTexture("assets/textures/ladder.png"));
        this->setZOrder(1);
        halfWidth = scale.x / 2.0f;
        halfHeight = scale.y / 2.0f;

        Ladder::createPhysicsBody();

        //Align sprite center with physicsbody
        sprite->setPosition(glm::vec3(position.x,position.y,0.0f));
        sprite->setScale(glm::vec3(halfWidth * 2, halfHeight * 2, 1.0f)); //full width and height
    }

    void Ladder::createPhysicsBody() {
        // 1. Create the Body (The "Anchor" for the logic)
        b2BodyDef bodyDef = b2DefaultBodyDef();
        bodyDef.type = b2_staticBody; // It stays in one spot
        bodyDef.position = {position.x, position.y};
        bodyDef.userData = this;
        body = b2CreateBody(physicsWorld, &bodyDef);

        // 2. Create the Shape (The "Ghost" Trigger)
        float sensorWidth = halfWidth;
        float sensorHeight = 2.0f;
        b2Polygon sensorBox = b2MakeBox(sensorWidth, sensorHeight);

        // 3. Configure as a SENSOR
        b2ShapeDef shapeDef = b2DefaultShapeDef();
        shapeDef.isSensor = true; // <--- THIS is what makes it a "Ghost"
        shapeDef.filter.categoryBits = gl3::PhysicsCategories::CATEGORY_SWITCH;
        shapeDef.filter.maskBits = gl3::PhysicsCategories::CATEGORY_PLAYER |
                                   gl3::PhysicsCategories::CATEGORY_BOX;

        // Attach it
        shape = b2CreatePolygonShape(body, &shapeDef, &sensorBox);
        b2Shape_SetUserData(shape, this);
        b2Shape_EnableSensorEvents(shape, true);
    }

    void Ladder::draw(Game *game) {
        Entity::draw(game);
    }

    void Ladder::onSensorEnter(Entity* other) {
        if (other->getType() == EntityType::Player) {
            Ship* player = static_cast<Ship*>(other);
            player->canClimb = true;


        }
    }

    void Ladder::onSensorExit(Entity* other) {
        if (other->getType() == EntityType::Player) {
            Ship* player = static_cast<Ship*>(other);
            player->canClimb= false;

            // Safety: Ensure they don't float away if they exit mid-climb
            b2Body_SetGravityScale(player->getBody(), 1.0f);
            std::cout << "Player exited ladder zone" << std::endl;
        }
    }



}