#pragma once
#include "../Sprite.h" //.. means go up one folder
#include "Floor.h"
#include "Entity.h"
#include <soloud.h>
#include <soloud_wav.h>

#include "Ladder.h"
#include "MirrorCube.h"
#include "PipeSwitch.h"
#include "Missile.h"
#include "SpeakerSwitch.h"
#include "box2d/math_functions.h"
#include "box2d/src/shape.h"
#include "../rendering/Texture.h"
#include "Stick.h"
#include "Vent.h"


namespace gl3 {
    class Sprite;
    enum class PlayerState {
        idle,
        accelerating,
        decelerating,
        continuousSpeed,
        climbingLadder,
        climbingBox,
        pushingBox
    };
    class Ship: public Entity {
    public:
        explicit Ship(Shader* shader, glm::vec3 position = glm::vec3(0.0f, 0.6f, 0.0f),
             glm::vec3 scale = glm::vec3(0.25f, 0.25f, 0.25f), int floorID = 0, b2WorldId physicsWorld = b2_nullWorldId);
        void update(Game *game, float deltaTime) override;
        void startContact(Entity* other) override;
        void endContact(Entity* other) override;
        void onSensorEnter(Entity* other) override;
        void onSensorExit(Entity* other) override;
        void setCurrentCube(MirrorCube* mirrorCube) {correspMirrorCube = mirrorCube;}
        void setStick(Stick *stick);
        void setCurrentLadder(Ladder* ladder) {currentLadder = ladder;}
        void setCurrentSpeakerSwitch(SpeakerSwitch* speakerSwitch) {currentSpeakerSwitch = speakerSwitch;}
        void setCurrentPipeSwitch(PipeSwitch* pipeSwitch) {currentPipeSwitch = pipeSwitch;}
        void setCorrespVent(Vent* vent) {correspVent = vent;}
        void setPlayerToBox (float PlayerToBox) {playertobox = PlayerToBox;}
        void setBoxPositionX(float boxPosX) {boxPositionX = boxPosX;}
        void setCanClimb(bool canclimb) {canClimb = canclimb;}
        void setBoxHeight(float height) {boxHeight = height;}
        bool getIsClimbing() {return isClimbing;}
        float getHalfHeight() {return playerHalfHeight;}
        bool getPushPullPressed() {return push_PullKeyPressed;}
        float getPlayerVelX() {return playerVelX;}
        float getHalfWidth() {return playerHalfWidth;}
        void setBoxDataValid(bool dataValid) {isBoxDataValid = dataValid;}
        void setFacingBox (bool isFacing) {isFacingBox = isFacing;}
        bool getIsDead() {return isDead;}
        void setIsDead(bool dead) {isDead = dead;}

        int getCurrentFloorId () { return floorID; }
        float getDirection() {return direction;}
        void kill(Game* game);



    protected:
        void createPhysicsBody() override;

    private:
        glm::vec3 playerPos;
        b2ShapeId shape;
        bool canClimb = false;
        bool isClimbing = false;
        bool push_PullKeyPressed = false;
        bool spaceJustPressed;
        bool canJump = false;
        float playerVelX;
        float playerHalfHeight;
        float playerHalfWidth;
        bool isGrounded = false;
        int groundContacts = 0;
        bool isDead = false;
        bool holdingDown = false;
        bool moveUp = false;
        float boxPositionX = 0.0f;
        float boxHeight = 0.0f;
        bool isFacingBox = false;
        bool isBoxDataValid = false;


        bool isOnBox = false;
        bool requestedJump = false;
        float climbTimer = 0.0f;
        glm::vec3 climbStartPos;
        glm::vec3 climbEndPos;
        bool shiftWasPressed = false;
        float playertobox = 0.0f;

        //ANIMATION
        std::vector<GLuint> jumpingFrames;
        std::vector<GLuint> pushingFrames;
        std::vector<GLuint> climbLadderFrames;
        std::vector<GLuint> climbBoxFrames;
        std::vector<GLuint> acceleratingFrames;
        std::vector<GLuint> deceleratingFrames;
        std::vector<GLuint> continuousSpeedFrames;

        //IDLE
        GLuint idleTexture;

        int animFrame = 0;          // Which frame index are we currently showing?
        float animTimer = 0.0f;     // Tracks time passed *within* the current frame
        const float frameDuration = 0.1f; // How long each frame stays on screen (0.1s = 10 FPS)


        PlayerState currentState = PlayerState::idle;
        PlayerState previousState;
        bool ismoving = false;


        MirrorCube* correspMirrorCube = nullptr;
        int floorID;
        Stick *myStick = nullptr;
        PipeSwitch* currentPipeSwitch = nullptr;
        Ladder* currentLadder = nullptr;
        Floor* currentFloor = nullptr;
        SpeakerSwitch* currentSpeakerSwitch = nullptr;
        Vent* correspVent = nullptr;
        gl3::Texture* texture; // The texture you loaded for the ship
        float sprintDuration = 1.0f;
        float sprintCooldown = 0.5f;
        float direction;
        SoLoud::Soloud audio;
        SoLoud::Wav firingSound;
        float translationSpeed = 3.0f;
        float rotationSpeed = 360.0f;
        b2Vec2 vel;
        bool spaceWasPressed;
        bool LMBwasDown = false;
        bool RMBwasDown = false;
        bool isAiming = false;
        bool F_wasPressed = false;
        bool E_wasPressed = false;

    };
}


