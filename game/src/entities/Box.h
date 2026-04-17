#pragma once
#include "Entity.h"
#include "Ship.h"
#include "../Sprite.h"
#include "box2d/types.h"

namespace gl3 {


    class Box: public Entity{
        public:
        explicit Box(Shader* shader, glm::vec3 position = glm::vec3(0,0,0), glm::vec3 scale = glm::vec3(0,0,0), glm::vec4 color = glm::vec4(1, 0, 0, 1), b2WorldId physicsWorld = b2_nullWorldId);
        void draw(Game *game) override;
        void startContact() override;
        void update(Game *game, float deltaTime) override;
        void endContact();
        [[nodiscard]] float GetHalfHeight() const;
        [[nodiscard]] float GetHalfWidth() const;
        //void DebugDraw();
        [[nodiscard]] EntityType getType() const override {
            return EntityType::Box;
        }





        protected:
        void createPhysicsBody() override;

    private:
        float halfWidth = 0.0f, halfHeight = 0.0f;
        bool isMovable = false;
        int playerContactCount = 0;
        float stillTime = 0.0f;              // how long the box has been still
        const float stillThreshold = 0.2f;   // time required before freezing again



    };

}


