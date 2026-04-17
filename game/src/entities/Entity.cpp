#include "Entity.h"

#include <iostream>

#include "../Game.h"

namespace gl3 {
    Entity::Entity(Shader* shader, glm::vec3 position, glm::vec3 scale, glm::vec4 color, b2WorldId physicsWorld, EntityType type) :
            shader(shader),
            position(position),
            scale(scale),
            color(color),
            physicsWorld(physicsWorld),
            body(b2_nullBodyId),
            shape(b2_nullShapeId),
            type(type)
    {}

    Entity::~Entity()
    {
        if(b2World_IsValid(physicsWorld))
        {
            if(b2Body_IsValid(body))
                b2DestroyBody(body);

        }
        delete sprite; // Clean up the pointer
    }

    void Entity::setPosition(const glm::vec3 &newPosition) {
        this->position = newPosition;


        // Update physics body
        if (body.index1 != 0) {  // Check if body is valid
            b2Body_SetTransform(body, b2Vec2{newPosition.x, newPosition.y}, b2Body_GetRotation(body)); //already sets sprite on the ground,if not, sprite falls onto ground
        }
    }

    void Entity::draw(Game *game) {
        // We pass zRotation here instead of 0
        auto mvpMatrix = game->calculateMvpMatrix(position, zRotation, scale);

        shader->use();
        shader->setMatrix("mvp", mvpMatrix);
        // ... draw calls
        sprite->draw(*shader,game, zRotation);
    }

    b2BodyId Entity::getBody() const {
        return body;
    }

    b2ShapeId Entity::getShape() const {
        return shape;
    }



    void Entity::updateBasedOnPhysics() {
        if(!b2Body_IsValid(body)) return;

        // 1. Get the Raw Physics Data (The "Truth")
        auto physicsTransform = b2Body_GetTransform(body);

        // 2. Sync the Entity's internal coordinates
        this->position.x = physicsTransform.p.x;
        this->position.y = physicsTransform.p.y;

        // 3. Convert Radians to Degrees for OpenGL
        this->zRotation = glm::degrees(b2Rot_GetAngle(physicsTransform.q));

        // 4. Force the Sprite to follow the Physics Center
        if (sprite != nullptr) {
            // This ensures the DRAW position is exactly the PHYSICS position
            sprite->setPosition(this->position);
            sprite->setRotation(this->zRotation);
        }
    }

    void Entity::startContact() {

    }
}
