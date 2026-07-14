#pragma once
#include "../Sprite.h"
#include "glm/vec3.hpp"
#include "../rendering/Shader.h"
#include "box2d/id.h"
#include "../EntityType.h"
#include "box2d/box2d.h"
#include "box2d/collision.h"

namespace gl3 {
    class Game;

    /**
     * @brief The base class for all interactive game objects.
     * * Inherit from this class to create custom objects (e.g., Robots, Doors, Enemies).
     * It provides the fundamental interface for the game loop, rendering, and physics interactions.
     */
    class Entity {
    public:
        /**
         * @brief Constructs a new Entity.
         * @param shader The shader to use for rendering.
         * @param position Initial world position.
         * @param scale Initial scale dimensions.
         * @param physicsWorld Reference to the Box2D physics world.
         * @param type The entity category.
         * @param initialRotation Starting rotation in degrees.
         * @param floorID The floor level for layer sorting.
         */
        explicit Entity(Shader* shader,
               glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
               glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f),
               b2WorldId physicsWorld = b2_nullWorldId,
               EntityType type = EntityType::None, float initialRotation = 0.0f,
               int floorID = 0);

        virtual ~Entity();

        /** @brief Returns the floor ID, used for sorting or floor-specific logic. */
        virtual int getFloorID() const { return -1; }

        /** * @brief Override this to define unique per-frame game logic.
         * @param game Pointer to the main Game instance.
         * @param deltaTime Time elapsed since the last frame.
         */
        virtual void update(Game *game, float deltaTime) {}

        /** @brief Renders the entity using its assigned sprite and shader. */
        virtual void draw(Game *game);

        // --- Getters and Setters ---
        [[nodiscard]] const glm::vec3 &getPosition() const { return position; }
        [[nodiscard]] const glm::vec3 &getScale() const { return scale; }
        virtual void setPosition(const glm::vec3 &position);

        /** @brief Sets Z-axis rotation and synchronizes it with the visual Sprite. */
        void setZRotation(float newRotation) {
            this->zRotation = newRotation;
            if (this->sprite != nullptr) {
                this->sprite->setRotation(newRotation);
            }
        }

        void setScale(const glm::vec3 &scale) { Entity::scale = scale; }
        void setZOrder(int order) { zOrder = order; }
        int getZOrder() const { return zOrder; }

        /** * @brief Calculates the final scale for rendering.
         * If a sprite is attached, it returns the sprite's scale; otherwise, it returns the entity's own scale.
         */
        virtual glm::vec3 getDrawScale() {
            return (sprite != nullptr) ? sprite->getScale() : this->scale;
        }

        // --- Physics and Collision Interface ---

        /** @brief Synchronizes the visual position with the physics body coordinates. */
        virtual void updateBasedOnPhysics();

        /** @brief Called when a collision begins. Override to handle gameplay triggers. */
        virtual void startContact(Entity* other);

        /** @brief Called when a collision ends. */
        virtual void endContact(Entity* other);

        /** @brief Resets the entity to its initial state. */
        virtual void resetState() {};

        /** @brief Triggered when entering a physics sensor volume. */
        virtual void onSensorEnter(Entity* other);

        /** @brief Triggered when exiting a physics sensor volume. */
        virtual void onSensorExit(Entity* other);

        /** @brief Returns the entity type for identification during gameplay. */
        [[nodiscard]] virtual EntityType getType() const { return type; }

        /** @brief Pauses entity physics and logic execution. */
        virtual void Freeze();

        /** @brief Resumes entity physics and logic execution. */
        virtual void UnFreeze();

        // --- Internal Methods ---
        /** @internal */
        virtual b2BodyId getBody() const;
        /** @internal */
        [[nodiscard]] b2ShapeId getShape() const;
        /** @internal */
        virtual Sprite *getSprite() { return sprite; }

    protected:
        /** @internal Internal engine-managed fields */
        int floorID;
        Sprite* sprite = nullptr;
        int zOrder = 0;
        EntityType type;

        /** @brief Pure virtual method: Subclasses must define how their physics body is built. */
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