#pragma once
#include "../Sprite.h" //.. means go up one folder
#include "Floor.h"
#include "Entity.h"
#include <soloud.h>
#include <soloud_wav.h>
#include "Ladder.h"
#include "Missile.h"
#include "SpeakerSwitch.h"
#include "box2d/math_functions.h"
#include "box2d/src/shape.h"
#include "../rendering/Texture.h"
#include "Stick.h"
#include "Vent.h"


namespace gl3 {
    class Sprite;
    class Ship: public Entity {
    public:
        explicit Ship(Shader* shader, glm::vec3 position = glm::vec3(0.0f, 0.6f, 0.0f),
             glm::vec3 scale = glm::vec3(0.25f, 0.25f, 0.25f), int floorID = 0, b2WorldId physicsWorld = b2_nullWorldId);
        void update(Game *game, float deltaTime) override;
        void startContact(Entity* other) override;
        void setStick(Stick *stick);
        void setCurrentLadder(Ladder* ladder) {currentLadder = ladder;}
        void setCurrentSpeakerSwitch(SpeakerSwitch* speakerSwitch) {currentSpeakerSwitch = speakerSwitch;}
        void setCorrespVent(Vent* vent) {correspVent = vent;}
        int getCurrentFloorId () { return floorID; }
        float getDirection() {return direction;}
        void kill(Game* game);

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


    protected:
        void createPhysicsBody() override;

    private:
        int floorID;
        Stick *myStick = nullptr;
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
        bool F_WasPressed = false;
        bool V_wasPressed = false;
        bool E_wasPressed = false;

    };
}


