#pragma once
#include "../Sprite.h"
#include "glm/vec3.hpp"
#include "../rendering/Shader.h"
#include "box2d/id.h"
#include "../EntityType.h"
#include "box2d/collision.h"

namespace gl3{
    class Game;

    class Entity {
    public:
        explicit Entity(Shader* shader,
               glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
               glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f),
               b2WorldId physicsWorld = b2_nullWorldId,
               EntityType type = EntityType::None, float initialRotation = 0.0f);

        virtual ~Entity();

        virtual void update(Game *game, float deltaTime) {}

        virtual void draw(Game *game);

        [[nodiscard]] const glm::vec3 &getPosition() const {return position; }
        //[[nodiscard]] float getZRotation() const { return zRotation; }
        [[nodiscard]] const glm::vec3 &getScale() const { return scale; }
        virtual void setPosition(const glm::vec3 &position);
        void setZRotation(float newRotation) {
            this->zRotation = newRotation;

            // Push the change to the Sprite immediately
            if (this->sprite != nullptr) {
                this->sprite->setRotation(newRotation);
            }
        }
        void setScale(const glm::vec3 &scale) { Entity::scale = scale; }
        void setZOrder(int order) { zOrder = order; }
        virtual b2BodyId getBody() const;
        [[nodiscard]] b2ShapeId getShape() const;
        int getZOrder() const { return zOrder; }
        virtual void updateBasedOnPhysics();
        virtual void startContact(Entity* other);
        virtual void endContact(Entity* other);
        virtual void resetState() {};
        virtual void onSensorEnter(Entity* other);
        virtual void onSensorExit(Entity* other);
        virtual Sprite *getSprite() { return sprite; }
        // Add this to your base Entity class
        virtual glm::vec3 getDrawScale() {
            // If the entity has a sprite, return the sprite's scale (which is flipped)
            // If not, just return the entity's own scale
            return (sprite != nullptr) ? sprite->getScale() : this->scale;
        }

        [[nodiscard]] virtual EntityType getType() const { return type; }
        virtual void Freeze();
        virtual void UnFreeze();


    protected:
        Sprite* sprite = nullptr;
        int zOrder = 0;
        EntityType type;
        virtual void createPhysicsBody() = 0;
        glm::vec3 position;
        float zRotation;
        glm::vec3 scale;
        glm::vec4 color;
        b2WorldId physicsWorld;
        b2BodyId body;
        b2ShapeId shape;
        Shader* shader;
    };
}



