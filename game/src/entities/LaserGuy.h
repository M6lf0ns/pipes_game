#pragma once
#include "Entity.h"
#include "../Game.h"

namespace gl3 {
    enum class LaserState {
        idle,
        firing
    };
    class LaserGuy: public Entity {
    public:
        explicit LaserGuy(Shader *shader, glm::vec3 position, glm::vec3 scale, b2WorldId physicsWorld);
        void update(Game *game, float deltaTime) override;
        void draw(Game *game) override;
        void updateAnimation(float deltaTime);
        void castRay();


    protected:
        void createPhysicsBody() override;
        private:
        float HalfHeight;
        float HalfWidth;

    private:
        const float eyeOffsetX = 0.25f; // Horizontal distance from character center to eyes
        const float eyeOffsetY = 0.1f; // Vertical distance from character center to eyes
        LaserState currentState = LaserState::idle;
        float stateTimer = 0.0f;
        int animFrame = 0;          // Which frame index are we currently showing?
        float animTimer = 0.0f;     // Tracks time passed *within* the current frame
        const float frameDuration = 0.1f; // How long each frame stays on screen (0.1s = 10 FPS)
        const int totalLaserFrames = 6;   // Let's assume your laser animation has 4 frames
        // Configuration Intervals (Adjust these to balance your level!)
        const float idleDuration = 3.0f;   // How long he waits between shots
        const float firingDuration = 2.0f; // How long the beam stays active
        float maxLaserRange = 15.0f;
        bool beamHitSomething = false;

        // Sprites for the 3 pieces
        Sprite* laserHeadSprite = nullptr;
        Sprite* laserBodySprite = nullptr;
        Sprite* laserTailSprite = nullptr;

        // Animation arrays (4 frames each)
        std::vector<GLuint> headFrames;
        std::vector<GLuint> bodyFrames;
        std::vector<GLuint> tailFrames;
        glm::vec2 currentHitPoint{0.0f, 0.0f};
    };

}

