#include "Ladder.h"
#include "../rendering/Texture.h"
#include "box2d/box2d.h"
#include "box2d/types.h"
#include <iostream>
#include "../PhysicsCategories.h"
#include "Ship.h"

namespace gl3 {
    Ladder::Ladder(Shader* shader, glm::vec3 position, glm::vec3 scale, b2WorldId physicsWorld)
      :Entity(shader,
          position,
          scale,
          physicsWorld,
          EntityType::Ladder)

    {
        this->sprite = new Sprite(position, scale, Texture::LoadTexture("assets/textures/ladder.png"));
        this->setZOrder(0);
        halfWidth = scale.x / 2.0f;
        halfHeight = scale.y / 2.0f;

        Ladder::createPhysicsBody();

        //Align sprite center with physicsbody
        sprite->setPosition(glm::vec3(position.x,position.y,0.0f));
        sprite->setScale(glm::vec3(halfWidth * 2, halfHeight * 2, 1.0f)); //full width and height
    }

    void Ladder::createPhysicsBody() {
        b2BodyDef bodyDef = b2DefaultBodyDef();
        bodyDef.type = b2_staticBody;
        bodyDef.position = {position.x, position.y};
        bodyDef.userData = this;
        body = b2CreateBody(physicsWorld, &bodyDef);

        // Make the sensor height exactly the same as the ladder.
        // If the floor is 0.1 thick and sits exactly at the ladder's top,
        // the player's feet will still be "touching" the top edge of the sensor.
        sensorHeight = (halfHeight * 2.0f) + 0.6f;
        b2Polygon sensorBox = b2MakeBox(halfWidth, sensorHeight / 2.0f);

        b2ShapeDef shapeDef = b2DefaultShapeDef();
        shapeDef.isSensor = true;
        shapeDef.filter.categoryBits = gl3::PhysicsCategories::CATEGORY_LADDER;
        shapeDef.filter.maskBits = gl3::PhysicsCategories::CATEGORY_PLAYER |
                                   gl3::PhysicsCategories::CATEGORY_BOX;
        shapeDef.userData = this;

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
            player->setCurrentLadder(this);


        }
    }

    void Ladder::onSensorExit(Entity* other) {
        if (other->getType() == EntityType::Player) {
            Ship* player = static_cast<Ship*>(other);
            player->canClimb= false;
            player->setCurrentLadder(nullptr);

            // Safety: Ensure they don't float away if they exit mid-climb
            b2Body_SetGravityScale(player->getBody(), 1.0f);
            std::cout << "Player exited ladder zone" << std::endl;
        }
    }



}