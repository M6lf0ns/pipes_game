#pragma once

#include "glm/vec3.hpp"
#include "../rendering/Shader.h"
#include "box2d/id.h"
#include "../EntityType.h"

namespace gl3{
    class Game;

    class Entity {
    public:
        explicit Entity(Shader shader,
               glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
               float zRotation = 0.0f,
               glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f),
               glm::vec4 color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
               b2WorldId physicsWorld = b2_nullWorldId,
               EntityType type = EntityType::None);

        virtual ~Entity();

        virtual void update(Game *game, float deltaTime) {}

        virtual void draw(Game *game);

        [[nodiscard]] const glm::vec3 &getPosition() const {return position; }
        [[nodiscard]] float getZRotation() const { return zRotation; }
        [[nodiscard]] const glm::vec3 &getScale() const { return scale; }
        virtual void setPosition(const glm::vec3 &position) { Entity::position = position; }
        void setZRotation(float zRotation) { Entity::zRotation = zRotation; }
        void setScale(const glm::vec3 &scale) { Entity::scale = scale; }
        [[nodiscard]] b2BodyId getBody() const;
        [[nodiscard]] b2ShapeId getShape() const;
        virtual void updateBasedOnPhysics();
        virtual void startContact();

        [[nodiscard]] EntityType getType() const { return type; }


    protected:
        EntityType type;
        virtual void createPhysicsBody() = 0;
        glm::vec3 position;
        float zRotation;
        glm::vec3 scale;
        glm::vec4 color;
        b2WorldId physicsWorld;
        b2BodyId body;
        b2ShapeId shape;
        Shader shader;
    };
}



