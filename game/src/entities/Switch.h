#pragma once
#include "Entity.h"
#include "box2d/src/joint.h"
#include "glm/vec3.hpp"
#include "../Sprite.h"
#include "Door.h"

namespace gl3 {
    class Switch: public Entity{
    public:
        explicit Switch(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 scale = glm::vec3(0.0f,0.0f,0.0f), glm::vec4 color = glm::vec4(1,1, 0, 1), b2WorldId physicsWorld = b2_nullWorldId);
        void draw(Game *game) override;
        void startContact() override;
        void endContact();
        [[nodiscard]] float getHalfHeight() const;
        [[nodiscard]] float getHalfWidth() const;

    protected:
        void createPhysicsBody() override;


    private:
        float halfHeight = 0.0f, halfWidth = 0.0f;
        Sprite sprite;
        bool isTouchingSomething = false;

    };
}
