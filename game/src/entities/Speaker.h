#pragma once
#include "Entity.h"
#include "tiny_gltf/tiny_gltf.h"

namespace gl3 {
    class Speaker: public Entity {
    public:
        explicit Speaker(Shader* shader, glm::vec3 position = glm::vec3(0,0,0), glm::vec3 scale = glm::vec3(0,0,0), b2WorldId physicsWorld = b2_nullWorldId);
        void onSensorEnter(Entity* other) override;
        void onSensorExit(Entity* other) override;
        void setCurrentTarget(Entity* target) {currentTarget = target;};
        void StartNoise();
        void CeaseNoise();
        void update(Game *game, float deltaTime) override;


    protected:
        void createPhysicsBody() override;

    private:
        Entity* currentTarget = nullptr;
        bool isActive = false;














    };
}



