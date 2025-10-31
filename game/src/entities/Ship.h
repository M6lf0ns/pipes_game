#pragma once

#include "Entity.h"
#include <soloud.h>
#include <soloud_wav.h>
#include "Missile.h"

namespace gl3 {
    class Ship: public Entity {
    public:
        explicit Ship(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
             float zRotation = 0.0f,
             glm::vec3 scale = glm::vec3(0.25f, 0.25f, 0.25f), b2WorldId physicsWorld = b2_nullWorldId);
        void update(Game *game, float deltaTime) override;
        void draw(Game *game) override;
        void updateBasedOnPhysics() override;
        void startContact() override;

    protected:
        void createPhysicsBody() override;

    private:
        SoLoud::Soloud audio;
        SoLoud::Wav firingSound;
        float translationSpeed = 3.0f;
        float rotationSpeed = 360.0f;
        const float timeBetweenShots = 0.1;
        float countdownUntilNextShot = timeBetweenShots;
        std::vector<std::unique_ptr<Missile>> missiles;
    };
}


