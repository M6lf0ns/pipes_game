#pragma once
#include "Entity.h"
#include "box2d/src/joint.h"
#include "glm/vec3.hpp"
#include "../Sprite.h"
#include "Door.h"
#include <set>

namespace gl3 {
    class Switch: public Entity{
    public:
        explicit Switch(Shader* shader, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 scale = glm::vec3(0.0f,0.0f,0.0f), b2WorldId physicsWorld = b2_nullWorldId);
        void onSensorEnter(Entity* other) override;
        void onSensorExit(Entity* other) override;
        //void update(Game *game, float deltaTime) override;
        [[nodiscard]] float getHalfHeight() const;
        [[nodiscard]] float getHalfWidth() const;
        void setTargetDoor(Door* door) { targetDoor = door; }

    protected:
        void createPhysicsBody() override;


    private:
        std::set<Entity*> occupants; // This replaces your int counters
        Door* targetDoor = nullptr; // Pointer to the door this switch controls
        float halfHeight = 0.0f, halfWidth = 0.0f;
        bool isTouchingSomething = false;
        int occupantCount = 0;

    };
}
