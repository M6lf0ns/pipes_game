#pragma once

#include "Entity.h"
#include "Missile.h"

namespace gl3 {
    class Enemy: public Entity {
    public:
        explicit Enemy(glm::vec3 position = glm::vec3(0, 0, 0), float zRotation = 0, float size = 1, b2WorldId physicsWorld = b2_nullWorldId);
        void update(Game *game, float deltaTime) override;
        void draw(Game *game) override;
        void startContact() override;

    protected:
        void createPhysicsBody() override;

    public:
        void updateBasedOnPhysics() override;

    private:
        float minSpeed = 0.05f;
        float maxSpeed = 0.1f;
        std::vector<std::unique_ptr<Missile>> missiles;
        const float timeBetweenShots = 0.1;
        float countdownUntilNextShot = timeBetweenShots;
    };
}



