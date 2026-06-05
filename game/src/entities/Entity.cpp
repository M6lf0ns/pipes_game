#include "Entity.h"

#include <iostream>

#include "../Game.h"

namespace gl3 {
    Entity::Entity(Shader* shader, glm::vec3 position, glm::vec3 scale, b2WorldId physicsWorld, EntityType type) :
            shader(shader),
            position(position),
            scale(scale),
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

        if (this->sprite) {
            this->sprite->setPosition(glm::vec3(newPosition.x, newPosition.y, 0.0f));
        }
    }

    void Entity::draw(Game *game) {
        if (sprite == nullptr) return;

        // 1. Get the VISUAL scale (flipped) instead of the physics scale
        // This calls the virtual function we added to the header
        glm::vec3 visualScale = this->getDrawScale();

        // 2. Calculate the matrix using that visual scale
        auto mvpMatrix = game->calculateMvpMatrix(position, zRotation, visualScale);

        // 3. Make sure the shader is ready
        shader->use();

        // 4. Send the matrix to the shader
        // IMPORTANT: Ensure "mvp" matches the name in your vertex shader exactly
        shader->setMatrix("mvp", mvpMatrix);

        // 5. Tell the sprite to do the actual OpenGL draw calls
        sprite->draw(*shader, mvpMatrix);
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

    void Entity::startContact(Entity* other) {

    }

    void Entity::endContact(Entity* other) {

    }
    void Entity::onSensorEnter(Entity* other) {

    }

    void Entity::onSensorExit(Entity* other) {

    }

    void Entity::Freeze() {

    }

    void Entity::UnFreeze() {

    }




}
