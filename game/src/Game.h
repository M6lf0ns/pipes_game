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


#include "soloud_wavstream.h"
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
            MainMenu,
            Ending
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
        bool getIsResetting() { return isResetting; }
        bool isShipValid();
        void InitAudio();
        void PlayMusic(SoLoud::WavStream& music, float volume = 0.7f);
        void StopMusic();
        void StopSFX(SoLoud::handle handle);
        SoLoud::handle PlaySfxWithHandle(SoLoud::Wav& sfx, float volume = 1.0f);
        void PlaySfx(SoLoud::Wav& sfx, float volume = 1.0f);
        SoLoud::Soloud& getAudio() { return audio; }

        void requestSceneSwitch(int ID);
        void switchScene (int ID);
        void setGoalReached(bool reachedGoal) {goalReached = reachedGoal;};
        SoLoud::WavStream& Game::getMusicForLevel(int levelID);


        void ShapesStartTouching(b2ShapeId shape_idA, b2ShapeId shape_idB);
        void ShapesStopTouching(b2ShapeId shape_idA, b2ShapeId shape_idB);
        void SensorStartsSensing(b2ShapeId sensorShapeId, b2ShapeId visitorShapeId);
        void SensorStopsSensing(b2ShapeId sensorShapeId, b2ShapeId visitorShapeId);

        SoLoud::Wav& getBotStunSFX() {return botStun;}
        SoLoud::Wav& getOpenVentSFX() {return openVent;}
        SoLoud::Wav& getFlipSwitchSFX() {return flipSwitch;}
        SoLoud::Wav& getPipeLandSFX() {return pipeLand;}
        SoLoud::Wav& getBoxPushPullSFX() {return boxPush_Pull;}
        SoLoud::Wav& getDoorOpenCloseSFX() {return doorOpen_Close;}
        SoLoud::Wav& getPlayerSpottedSFX() {return playerSpotted;}
        SoLoud::Wav& getPipeRotatedSFX() {return pipeRotated;}
        SoLoud::Wav& getLaserFiredSFX() {return laserFired;}
        SoLoud::Wav& getLaserGuyHitSFX() {return laserGuyHit;}
        SoLoud::Wav& getDogWalkSFX() {return dogWalk;}
        SoLoud::Wav& getpickUpStick() {return pickUpStick;}


    private:
        bool gameOverSoundPlayed = false;
        SoLoud::Soloud audio;
        SoLoud::WavStream menuMusic;
        SoLoud::WavStream levelMusic;
        SoLoud::WavStream levelMusic2;
        SoLoud::WavStream levelMusic3;
        SoLoud::WavStream gameOverMusic;
        SoLoud::WavStream gameEndingMusic;
        SoLoud::Wav botStun;
        SoLoud::Wav openVent;
        SoLoud::Wav flipSwitch;
        SoLoud::Wav pipeLand;
        SoLoud::Wav boxPush_Pull;
        SoLoud::Wav doorOpen_Close;
        SoLoud::Wav playerSpotted;
        SoLoud::Wav pipeRotated;
        SoLoud::Wav laserFired;
        SoLoud::Wav laserGuyHit;
        SoLoud::Wav dogWalk;
        SoLoud::Wav pickUpStick;
        SoLoud::Wav gameOver;
        SoLoud::handle currentMusicHandle = 0;
        bool isResetting = false;
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

        std::unique_ptr<SoLoud::Wav> backgroundMusic;
        float lastFrameTime = 1.0f/60;
        float deltaTime = 1.0f/60;
        b2WorldId physicsWorld;
        float accumulator = 0.f;



    };
}



