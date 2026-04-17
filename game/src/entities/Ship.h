#pragma once
#include "../Sprite.h" //.. means go up one folder
#include "Entity.h"
#include <soloud.h>
#include <soloud_wav.h>
#include "Missile.h"
#include "box2d/math_functions.h"
#include "box2d/src/shape.h"
#include "tiny_gltf/tiny_gltf.h"
#include "../rendering/Texture.h"
#include "Stick.h"


namespace gl3 {
    class Sprite;
    class Ship: public Entity {
    public:
        explicit Ship(Shader* shader, glm::vec3 position = glm::vec3(0.0f, 0.6f, 0.0f),
             glm::vec3 scale = glm::vec3(0.25f, 0.25f, 0.25f), b2WorldId physicsWorld = b2_nullWorldId);
        void update(Game *game, float deltaTime) override;
        void startContact() override;
        void setStick(Stick *stick);
        void pickUpStick(Stick *stick);
        void kill(Game* game);

        static glm::vec3 playerPos;
        b2ShapeId shape;
        b2ShapeId footSensorShape;
        static bool push_PullKeyPressed;
        static float playerVelX;
        static float playerHalfHeight;
        static float playerHalfWidth;
        bool isGrounded = false;
        int groundContacts = 0;
        bool isDead = false;


    protected:
        void createPhysicsBody() override;

    private:

        Stick *myStick = nullptr;
        gl3::Texture* texture; // The texture you loaded for the ship
        float sprintDuration = 1.0f;
        float sprintCooldown = 0.5f;
        SoLoud::Soloud audio;
        SoLoud::Wav firingSound;
        float translationSpeed = 3.0f;
        float rotationSpeed = 360.0f;
        b2Vec2 vel;
        static bool spaceWasPressed;
        bool LMBwasDown = false;
        bool RMBwasDown = false;
        bool isAiming = false;
    };
}


