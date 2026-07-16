#pragma once
#include "Entity.h"
#include "Floor.h"
#include "soloud.h"
#include "Vent.h"
#include "../Sprite.h"
#include "box2d/collision.h"

namespace gl3 {
    // Forward Declaration
    class Dog;
    class Stick : public Entity {
    public:
        Stick(Shader* shader, glm::vec3 position = glm::vec3(0,0,0), glm::vec3 scale = glm::vec3(0.0f,0.0f,0.0f), int floorID = 0, b2WorldId physicsWorld = b2_nullWorldId);
        void update(Game *game, float deltaTime) override;
        void updateHoldingPosition(glm::vec3 playerPos);
        void draw(Game *game) override;
        void simulateThrow(Game *game, glm::vec3 bodyPos, float direction);
        void throwStick(Game *game, glm::vec3 pos, float direction);
        void pickUpStick(Game *game, glm::vec3 pos, float direction);
        void dropStick(glm::vec3 pos, float direction);
        void drawTrajectory(Game *game);
        void setAiming( bool isAiming);
        void setDawg(Dog* dog);
        Dog* getDawg() {return dawg;}
        bool getCanBePickedUp() {return canBePickedUp;}
        bool getIsFollowingPlayer() {return isFollowingPlayer;}
        void setIsFollowingPlayer(bool followsPlayer) {isFollowingPlayer = followsPlayer;}
        void teleport(Game* game, glm::vec3 correspVentPos);
        void startContact(Entity* other) override;
        void endContact(Entity* other) override;
        void setFloorID(int floor) {floorID = floor;}
        int getFloorID() const override {return floorID;}
        ~Stick() override;




    protected:
        void createPhysicsBody() override;

    private:
        SoLoud::Soloud audio;
        bool landSoundPlayed = false;
        bool wasTeleporting = false;
        int floorID;
        bool isVisible = true;
        Dog* dawg = nullptr;
        bool hasLanded = false;
        bool hasBeenThrown = false;
        bool isFollowingPlayer = false;
        bool canBePickedUp = false;
        glm::vec3 playerPos;
        float impulse;
        bool isAiming = false;
        float mass;
        float maxTime = 1.5f;
        float timer = 0.0f;
        bool isTimerActive = false;
        std::vector<glm::vec3> trajectoryPoints;
        unsigned int vao, vbo;
        Floor* floor = nullptr;
        Vent* exitVent = nullptr;
        Vent* entryVent = nullptr;






    };
}
