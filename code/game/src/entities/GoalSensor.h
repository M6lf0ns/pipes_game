#pragma once
#include "Entity.h"

namespace gl3 {
    class GoalSensor: public Entity{
        public:
            explicit GoalSensor(Shader* shader, glm::vec3 position = glm::vec3(0,0,0), glm::vec3 scale = glm::vec3(0,0,0), b2WorldId physicsWorld = b2_nullWorldId);
            void onSensorEnter(Entity *other) override;
            void onSensorExit(Entity *other) override;
            void update(Game *game, float deltaTime) override;

    protected:
        void createPhysicsBody() override;
        bool goalReached = false;
        float delayTimer = 2.0f;
        bool isCountingDownTimer = false;



    };



}




