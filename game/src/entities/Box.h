#pragma once
#include "Entity.h"
#include "../Sprite.h"

namespace gl3 {

    class Box: public Entity{
        public:
        explicit Box(glm::vec3 position = glm::vec3(0,0,0), glm::vec3 scale = glm::vec3(0,0,0), glm::vec4 color = glm::vec4(1, 0, 0, 1), b2WorldId physicsWorld = b2_nullWorldId);
        void draw(Game *game) override;
        void startContact() override;
        void updateBasedOnPhysics() override;
        [[nodiscard]] float GetHalfHeight() const;
        void DebugDraw();


        protected:
        void createPhysicsBody() override;

    private:
        Sprite sprite;
        float halfWidth = 0.0f, halfHeight = 0.0f;



    };

}


