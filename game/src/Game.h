#pragma once
#include <memory>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <soloud.h>
#include <soloud_wav.h>
#include "entities/Ship.h"
#include <box2d/box2d.h>
#include "Sprite.h"
#include "entities/Dog.h"
#include "entities/Door.h"
#include "entities/Floor.h"
#include "entities/Switch.h"
#include "rendering/Texture.h"

namespace gl3 {
    class Game {
        enum Layers {
            BACKGROUND = -10,
            MIDGROUND = 0,
            PLAYER = 5,
            FOREGROUND = 10,
            UI = 100
        };
    public:
        Game(int width, int height, const std::string &title);
        virtual ~Game();
        void run();
        glm::mat4 calculateMvpMatrix(glm::vec3 position, float zRotationInDegrees, glm::vec3 scale);
        Shader* getSpriteShader() { return spriteShader.get(); }
        Ship *getShip() { return ship; }
        GLFWwindow *getWindow() { return window; }
        const b2WorldId getPhysicsWorld() const;
        const b2Vec2 getGravity() const;
        float aspect = 0.0f;
        // Define the PTM Ratio as a public constant
        // Based on your 720p window and zoom=2.0f, the ratio is 180:1
        static constexpr float PTM_RATIO = 100.0f;
        bool isGameOver = false;
        void drawGameOverOverlay();


        void ShapesStartTouching(b2ShapeId shape_idA, b2ShapeId shape_idB);
        void ShapesStopTouching(b2ShapeId shape_idA, b2ShapeId shape_idB);
        void SensorStartsSensing(b2ShapeId sensorShapeId, b2ShapeId visitorShapeId);
        void SensorStopsSensing(b2ShapeId sensorShapeId, b2ShapeId visitorShapeId);

    private:
        static b2WorldDef worldDef;
        bool firstFrame = true;
        std::unique_ptr<gl3::Shader> spriteShader;
        static void framebuffer_size_callback(GLFWwindow *window, int width, int height);
        void update();
        void draw();
        void updateDeltaTime();
        void updatePhysics();
        void resetGame();

        // Keep it simple
        gl3::Sprite* deathOverlay = nullptr;
        GLuint deathTexID;
        gl3::Sprite* deathText = nullptr;


        GLFWwindow *window = nullptr;

        float gameOverDuration = 1.0f;
        float gameOverTimer = 0.0f;

        std::vector<std::unique_ptr<Entity>> entities;
        Ship *ship = nullptr;
        Door *doorptr = nullptr;
        int boxOnPlateCount = 0;
        int playerOnPlateCount = 0;
        SoLoud::Soloud audio;
        std::unique_ptr<SoLoud::Wav> backgroundMusic;
        float lastFrameTime = 1.0f/60;
        float deltaTime = 1.0f/60;
        b2WorldId physicsWorld;
        float accumulator = 0.f;



    };
}



