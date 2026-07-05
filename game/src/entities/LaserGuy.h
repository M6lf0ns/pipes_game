#pragma once
#include "Entity.h"
#include "../Game.h"

namespace gl3 {
    enum class LaserState {
        idle,
        firing
    };
    enum class MirrorInteraction {
        None = 0,
        Turn = 1,       // 90-degree corner
        Reversal = 2, // 180-degree U-turn
        PassThrough = 3,
        Hit = 4
    };

    class LaserGuy: public Entity {
    public:
        explicit LaserGuy(Shader *shader, glm::vec3 position, glm::vec3 scale, b2WorldId physicsWorld, float startAngle);
        void update(Game *game, float deltaTime) override;
        void draw(Game *game) override;
        void updateAnimation(float deltaTime);
        void castRay();
        bool isParallelToPipe(glm::vec2 currentDir,MirrorCube* hitCube) ;
        MirrorCube* getCubeAtSegment(int i);
        bool isPassingThrough() { return isPassingthrough; }
        // Add to LaserGuy class
        // bool isDying = false;
        // float deathTimer = 0.0f;
        // bool isVisible = true;


    protected:
        void createPhysicsBody() override;
        private:
        float HalfHeight;
        float HalfWidth;

    private:

        float hitEntityWidth = 0.0f;
        bool rayTerminated = false;
        float incoming = 0.0f;
        glm::vec2 reflected = glm::vec2(0.0f);
        bool isRaycastReady = false;
        std::vector<bool> junctionProcessed;
        std::vector<glm::vec2> outboundPoints;
        std::vector<glm::vec2> returnPoints;
        // You might want to track mirror hits separately too:
        std::vector<bool> outboundMirrors;
        std::vector<bool> returnMirrors;

        float startAngle;
        int debugBodyDrawCount = 0;
        // Inside LaserGuy.h
        std::vector<glm::vec2> laserPoints;
        std::vector<int> segmentEndsOnMirror; // <--- ADD THIS LINE!
        std::vector<EntityType> segmentHitTypes;
        const float eyeOffsetX = 0.2f; // Horizontal distance from character center to eyes
        const float eyeOffsetY = 0.2f; // Vertical distance from character center to eyes
        LaserState currentState = LaserState::idle;
        MirrorInteraction currentInteraction = MirrorInteraction::None;
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
        bool isParallelToMirror = false;

        float laserThickness = 0.2f;
        float headWidth = 0.4f;
        float tailWidth = 0.4f;
        MirrorCube* hitCube = nullptr;
        bool isPassingthrough = false;

        float outgoingDir = 0.0f;
        std::vector<Entity*> hitEntities; // Stores the object hit at each index

        // Sprites for the 3 pieces
        Sprite* laserHeadSprite = nullptr;
        Sprite* laserBodySprite = nullptr;
        Sprite* laserTailSprite = nullptr;
        Sprite* laserCornerSprite = nullptr;
        // MirrorCube* hitCube = nullptr;





        // Animation arrays (4 frames each)
        std::vector<GLuint> headFrames;
        std::vector<GLuint> bodyFrames;
        std::vector<GLuint> tailFrames;
        std::vector<GLuint> edgeFrames;
        glm::vec2 currentHitPoint{0.0f, 0.0f};
    };

}

