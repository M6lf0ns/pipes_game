#pragma once
#include "../Sprite.h" //.. means go up one folder
#include "Entity.h"
#include <soloud.h>
#include <soloud_wav.h>
#include "Missile.h"
#include "box2d/math_functions.h"
#include "box2d/src/shape.h"


namespace gl3 {
    class Ship: public Entity {
    public:
        explicit Ship(glm::vec3 position = glm::vec3(0.0f, 0.6f, 0.0f),
             float zRotation = 0.0f,
             glm::vec3 scale = glm::vec3(0.25f, 0.25f, 0.25f), b2WorldId physicsWorld = b2_nullWorldId);
        void update(Game *game, float deltaTime) override;
        void draw(Game *game) override;
        void updateBasedOnPhysics() override;
        void startContact() override;
        void setPosition(const glm::vec3 &position) override;
        static glm::vec3 playerPos;
        b2ShapeId shape;
        static bool push_PullKeyPressed;
        static float playerVelX;
        static float shipHalfHeight;


    protected:
        void createPhysicsBody() override;

    private:

        Sprite sprite;
        SoLoud::Soloud audio;
        SoLoud::Wav firingSound;
        float translationSpeed = 3.0f;
        float rotationSpeed = 360.0f;

        b2Vec2 vel;
        static bool spaceWasPressed;
    };
}


