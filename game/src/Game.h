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
#include <functional>
#include <map>


#include "levels/Scene.h"
#include "Sprite.h"
#

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
        enum GameState {
            Paused,
            GameOver,
            Playing,
            MainMenu
        };
    public:
        Game(int width, int height, const std::string &title);
        virtual ~Game();
        void run();
        void saveGame();
        int loadSavedLevel();
        bool getIsGameOver() {return isGameOver;}
        glm::mat4 calculateMvpMatrix(glm::vec3 position, float zRotationInDegrees, glm::vec3 scale);
        glm::mat4 calculateUIMvPMatrix(glm::vec3 position, glm::vec3 scale);
        Ship *getShip() { return ship; }
        GLFWwindow *getWindow() { return window; }
        const b2WorldId getPhysicsWorld() const;
        const b2Vec2 getGravity() const;
        int getIdToLoad() { return idToLoad; }
        void setPendingLevelID(int ID) { pendingLevelID = ID; }
        int getCurrentLevelID() const { return idToLoad; }
        float getDeltaTime() { return deltaTime; }
        float aspect = 0.0f;
        void setShip(Ship *player) { this->ship = player; }
        // Define the PTM Ratio as a public constant
        // Based on your 720p window and zoom=2.0f, the ratio is 180:1
        static constexpr float PTM_RATIO = 100.0f;

        void requestSceneSwitch(int ID);
        void switchScene (int ID);
        void setGoalReached(bool reachedGoal) {goalReached = reachedGoal;};


        void ShapesStartTouching(b2ShapeId shape_idA, b2ShapeId shape_idB);
        void ShapesStopTouching(b2ShapeId shape_idA, b2ShapeId shape_idB);
        void SensorStartsSensing(b2ShapeId sensorShapeId, b2ShapeId visitorShapeId);
        void SensorStopsSensing(b2ShapeId sensorShapeId, b2ShapeId visitorShapeId);

    private:
        bool isGameOver = false;
        bool rKeyWasPressed = false;
        float endGameTimer = 0.0f;
        bool isWaitingToEnd = false;
        float delayTimer = 0.0f;
        bool isWaitingToSwitch = false;
        bool goalReached = false;
        std::string levelInputBuffer = "";
        bool inputMode = false;
        bool numKeysWasPressed[10] = {false}; // Tracks state for keys 0-9
        bool ESCWasPressed = false;
        int pendingLevelID = -1; // -1 means no switch requested
        static b2WorldDef worldDef;
        bool firstFrame = true;
        std::unique_ptr<gl3::Shader> spriteShader;
        static void framebuffer_size_callback(GLFWwindow *window, int width, int height);
        void update();
        void draw();
        void updateDeltaTime();
        void resetGame();
        GameState currentState = GameState::MainMenu;
        Sprite* menuSprite = nullptr;
        std::unique_ptr<Scene> currentScene;

        // Keep it simple
        gl3::Sprite* deathOverlay = nullptr;
        GLuint deathTexID;
        gl3::Sprite* deathText = nullptr;


        GLFWwindow *window = nullptr;

        float gameOverDuration = 1.0f;
        float gameOverTimer = 0.0f;

        // A map that connects an ID to a function that returns a new Scene
        std::map<int, std::string> levelFileRegistry;
        std::vector<std::unique_ptr<Entity>> entities;
        Scene* currentScenePtr = nullptr;
        Ship *ship = nullptr;
        int boxOnPlateCount = 0;
        int playerOnPlateCount = 0;
        int idToLoad = 0;
        bool levelLoaded = false;
        bool oneWasPressed = false;
        SoLoud::Soloud audio;
        std::unique_ptr<SoLoud::Wav> backgroundMusic;
        float lastFrameTime = 1.0f/60;
        float deltaTime = 1.0f/60;
        b2WorldId physicsWorld;
        float accumulator = 0.f;



    };
}



