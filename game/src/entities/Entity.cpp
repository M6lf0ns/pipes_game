#include "Entity.h"

#include <iostream>

#include "../Game.h"

namespace gl3 {
    Entity::Entity(Shader shader, glm::vec3 position, float zRotation, glm::vec3 scale, glm::vec4 color, b2WorldId physicsWorld, EntityType type) :
            shader(std::move(shader)),
            position(position),
            zRotation(zRotation),
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
    }

    void Entity::draw(Game *game) {
        auto mvpMatrix = game->calculateMvpMatrix(position, zRotation, scale);

        // // DEBUG: Check what's being set
        // std::cout << "ENTITY MVP SET - Position: (" << position.x << ", " << position.y << ")" << std::endl;
        // std::cout << "Camera target: (" << Ship::playerPos.x << ", " << Ship::playerPos.y << ")" << std::endl;
        // shader.use();
        // shader.setMatrix("mvp", mvpMatrix);
        // shader.setVector("color", color);

    }

    b2BodyId Entity::getBody() const {
        return body;
    }

    b2ShapeId Entity::getShape() const {
        return shape;
    }



    void Entity::updateBasedOnPhysics() {
        if(!b2Body_IsValid(body))
            return;

        auto physicsTransform = b2Body_GetTransform(body);

        position.x = physicsTransform.p.x;
        position.y = physicsTransform.p.y;

        zRotation = glm::degrees(b2Rot_GetAngle(physicsTransform.q));

    }

    void Entity::startContact() {

    }
}
