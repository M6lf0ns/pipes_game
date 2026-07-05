#pragma once
#include "Entity.h"
#include "Ship.h"
#include "Floor.h"
#include "../Sprite.h"
#include "box2d/collision.h"


namespace gl3 {

    class SecBot: public Entity {
        enum Botstate {
            patrol,
            chase,
            search,
            freeze
        };
        public:
        explicit SecBot(Shader* shader, glm::vec3 position = glm::vec3(0,0,0), glm::vec3 scale = glm::vec3(0,0,0),float rotation = 0.0f, int floorID = 0, b2WorldId physicsWorld = b2_nullWorldId);
        void update(Game *game, float deltaTime) override;
        void startContact(Entity* other) override;
        void setShip(Ship* sship) {ship = sship;}
        int getCurrentFloorId () { return floorID; }
        void checkForGroundAhead();
        bool canReachPlayer();
        void Freeze() override;
        // void draw(Game *game) override;
        void UnFreeze() override;
        // ~SecBot() override;

        // Public methods called by Game::SensorStartsSensing
        void castRay();


    protected:
    void createPhysicsBody() override;
        //void debugCheckAABBOverlap();

    private:
        std::vector<GLuint> patrolFrames;
        std::vector<GLuint> chaseFrames;
        std::vector<GLuint> searchFrames;
        int animFrame = 0;          // Which frame index are we currently showing?
        float animTimer = 0.0f;     // Tracks time passed *within* the current frame
        const float frameDuration = 0.1f; // How long each frame stays on screen (0.1s = 10 FPS)

        float rotationAng;
        Floor* currentFloor = nullptr;
        Ship* ship = nullptr;
        float detectionTimer = 0.0f;
        float TIME_TO_KILL = 0.5f; // Half a second of being seen before death
        void flipSpriteDirection();
        bool isTurning = false;
        float turnTimer = 0.7f;
        float timeRemainingBeforeTurn;
        // 1. Tracks the current direction of patrol movement
        //    1.0f for moving right, -1.0f for moving left
        float patrolDirection = 1.0f;
        float botRadius;
        b2Circle circleGeometry;
        bool playerSpotted = false;
        Botstate currentState = Botstate::patrol;
        float forgetTimer = 1.0f;
        float forgetCountdown = 0.0f;
        bool canSeePlayer = false;
        bool isViewBlocked = false;
        float lastKnownX = 0.0f;
        int floorID;

        // 3. (Optional, but good practice) Speed constant
        const float PATROL_SPEED = 0.5f; // Example speed in meters/second
        bool isFrozen = false;


    };
}



