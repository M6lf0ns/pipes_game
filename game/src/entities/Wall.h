#pragma once
#include "Entity.h"
#include "box2d/box2d.h"

namespace gl3 {

    class Wall : public Entity {
    public:
        // Constructor inherits from Entity
        explicit Wall(Shader* shader, glm::vec3 position, glm::vec3 scale, b2WorldId physicsWorld);

        // Override createPhysicsBody to define static wall properties
        void createPhysicsBody() override;

        // Walls don't typically need movement or complex contact logic
        void update(Game* game, float deltaTime) override {}
        void startContact(Entity* other) override {}
        void endContact(Entity* other) override {}
    };

} // namespace gl3