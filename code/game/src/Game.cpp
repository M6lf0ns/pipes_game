#define GLM_ENABLE_EXPERIMENTAL
#include "Game.h"
#include <stdexcept>
#include <random>
#include <iostream>
#include "Assets.h"
#include "box2d/box2d.h"
#include "entities/Box.h"
#include "glm/gtx/string_cast.hpp"
#include "rendering/Texture.h"
#include "rendering/Shader.h"
#include "entities/Switch.h"
#include <algorithm> //required for sort
#include <fstream>
#include <filesystem>
namespace fs = std::filesystem;

#include "entities/Dog.h"


namespace gl3 {

    b2WorldDef Game::worldDef = b2DefaultWorldDef();


    void Game::framebuffer_size_callback(GLFWwindow *window, int width, int height) {
        glViewport(0, 0, width, height);
    }

    Game::Game(int width, int height, const std::string &title) {
        if (!glfwInit()) {
            throw std::runtime_error("Failed to initialize glfw");
        }
        aspect = static_cast<float>(width) / static_cast<float>(height);

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

        window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
        if (window == nullptr) {
            throw std::runtime_error("Failed to create window");
        }

        glfwMakeContextCurrent(window);

        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

        // ADD THIS LINE - Enable VSync to lock to monitor refresh rate
        glfwSwapInterval(1); //makes box jitter go away BUT when pulling box makes box run after you
        gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
        if (glGetError() != GL_NO_ERROR) {
            throw std::runtime_error("gl error");
        }

        // Enable blending for transparent textures
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


        InitAudio();
        audio.setGlobalVolume(1.0f);
        PlayMusic(menuMusic);

        // Create the physics world
        worldDef = b2DefaultWorldDef();
        worldDef.gravity = b2Vec2{0.0f, -9.8f};
        physicsWorld = b2CreateWorld(&worldDef);
        levelFileRegistry[1] = resolveAssetPath("levels/level1.txt").string();
        levelFileRegistry[2] = resolveAssetPath("levels/level2.txt").string();
        levelFileRegistry[3] = resolveAssetPath("levels/level3.txt").string();
        levelFileRegistry[4] = resolveAssetPath("levels/level4.txt").string();
        levelFileRegistry[5] = resolveAssetPath("levels/level5.txt").string();
        levelFileRegistry[6] = resolveAssetPath("levels/level6.txt").string();
        levelFileRegistry[7] = resolveAssetPath("levels/level7.txt").string();
        levelFileRegistry[8] = resolveAssetPath("levels/level8.txt").string();
        levelFileRegistry[9] = resolveAssetPath("levels/level9.txt").string();
        levelFileRegistry[10] = resolveAssetPath("levels/level10.txt").string();
        levelFileRegistry[11] = resolveAssetPath("levels/level11.txt").string();
        levelFileRegistry[12] = resolveAssetPath("levels/level12.txt").string();
        levelFileRegistry[13] = resolveAssetPath("levels/level13.txt").string();
        levelFileRegistry[14] = resolveAssetPath("levels/level14.txt").string();
        levelFileRegistry[15] = resolveAssetPath("levels/level15.txt").string();
        levelFileRegistry[16] = resolveAssetPath("levels/level16.txt").string();
        levelFileRegistry[17] = resolveAssetPath("levels/level17.txt").string();
        levelFileRegistry[18] = resolveAssetPath("levels/level18.txt").string();
        levelFileRegistry[19] = resolveAssetPath("levels/level19.txt").string();
        // Simply set state! No sprite creation or texture file reads here.
        this->idToLoad = 1; // <-- CHANGE THIS FROM 0 TO 1
        this->menuSprite = nullptr;
        this->currentState = GameState::MainMenu;
    }

    Game::~Game() {
        glfwTerminate();
    }

    glm::mat4 Game::calculateMvpMatrix(glm::vec3 position, float zRotation, glm::vec3 scale) {
        // 1. Model Matrix
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        model = glm::rotate(model, glm::radians(zRotation), glm::vec3(0.0f, 0.0f, 1.0f));

        // This 'scale' parameter is the most important part!
        model = glm::scale(model, scale);
        float cameraX = 0.0f;

        // 2. View Matrix - The Smooth Follow
        if (ship != nullptr) {
            float targetX = ship->getPosition().x;

            // SMOOTHING: Move 10% of the distance each frame
            cameraX += (targetX - cameraX) * 0.1f;

            // CLAMPING: Prevent it from showing the "void"
            cameraX = std::clamp(cameraX, 0.0f, 20.0f);
        }
        float cameraY = 1.0f;
        glm::mat4 view = glm::lookAt(
            glm::vec3(cameraX, cameraY, 1.0f),
            glm::vec3(cameraX, cameraY, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f)
        );

        // 3. Projection Matrix
        float zoom = 2.0f;
        glm::mat4 projection = glm::ortho(-zoom * aspect, zoom * aspect, -zoom, zoom, 0.1f, 100.0f);

        return projection * view * model;
    }

    void Game::run() {
        unsigned int VAO;
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        std::mt19937 randomNumberEngine{
            static_cast<unsigned int>(std::chrono::steady_clock::now().time_since_epoch().count())
        };
        std::uniform_real_distribution positionDist{-2.0, 2.0};
        std::uniform_real_distribution scaleDist{0.2, 0.6};
        std::uniform_real_distribution colorDist{0.0, 1.0};
        for (auto i = 0; i < 10; ++i) {
            auto randomPosition = glm::vec3(static_cast<float>(positionDist(randomNumberEngine) * 1.5),
                                            static_cast<float>(positionDist(randomNumberEngine) * 1.5), 0);
            auto randomScale = static_cast<float>(scaleDist(randomNumberEngine));
            auto c = colorDist(randomNumberEngine);
            auto randomColor = glm::vec4(c, c, c, 1.0f);
        }

        this->switchScene(idToLoad);

        glfwSetTime(1.0 / 60);

        while (!glfwWindowShouldClose(window)) {
            updateDeltaTime();
            // ADD THIS TRACE PRINT
            if (glfwWindowShouldClose(window)) {
                std::cout << "[CRITICAL] Window close flag detected! Current State: " << (int)currentState << std::endl;
            }



            audio.update3dAudio();
            update();
            // Inside run() loop
            if (currentMusicHandle != 0) {
                if (audio.isValidVoiceHandle(currentMusicHandle)) {
                    // Voice is active
                } else {
                    std::cout << "[AUDIO] Warning: Current music handle is invalid/not playing!" << std::endl;
                }
            }

            // Only update the level scenes if we are actively playing
            if (currentState == GameState::Playing && currentScene) {
                currentScene->update(deltaTime);
                currentScene->updatePhysics(deltaTime);
            }

            draw();

            if (pendingLevelID != -1 && !levelLoaded) {
                switchScene(pendingLevelID);
                idToLoad = pendingLevelID;
                pendingLevelID = -1;
            }

            glfwPollEvents();
        }

        if ((currentState == GameState::Playing ||
      currentState == GameState::Paused  ||
      currentState == GameState::GameOver) && currentScene)
        {
            saveGame();
        }


        glDeleteVertexArrays(1, &VAO);
    }


    void Game::update() {

        if (currentState == GameState::Ending) {
            if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
                currentState = GameState::MainMenu;


                StopMusic();
                PlayMusic(menuMusic);

            }
            if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
                glfwSetWindowShouldClose(window, true);
            }
            return;
        }
        bool justPressedESC = glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS;

        if (justPressedESC) {
            if (!ESCWasPressed) {
                if (currentState == GameState::MainMenu || currentState == GameState::Paused || currentState == GameState::GameOver) {
                    std::cout << "[GAME] Exiting from Main Menu!" << std::endl;
                    glfwSetWindowShouldClose(window, true);
                    return;
                }
                ESCWasPressed = true; // Lock the latch instantly


                if (currentState == GameState::Playing) {
                    std::cout << "[GAME] Paused!" << std::endl;
                    currentState = GameState::Paused;
                    audio.setVolume(currentMusicHandle, 0.2f); // "Duck" the music to sound muffled
                     // Stop processing this frame completely
                }
            }
        } else {
            ESCWasPressed = false; // Reset the latch only when they let go
        }

        // --- STATE GUARDS ---
        if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
            if (currentState == GameState::MainMenu || currentState == GameState::Paused) {
                currentState = GameState::Playing;
                PlayMusic(getMusicForLevel(idToLoad));
                return;
            }

        }

        if (!isShipValid()) {
            std::cout << "[GAME] Ship is invalid!" << std::endl;

        }


        if (ship != nullptr && ship->getIsDead()) {
            std::cout << "IDTOLOAD IS " << idToLoad << std::endl;
            isGameOver = true;
        }

        // 1. Move the Reset Check HERE (Before the Game Over return)
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
            if (!rKeyWasPressed) {
                // Check both states
                if (currentState == GameState::GameOver || currentState == GameState::Paused) {
                    resetGame();
                    rKeyWasPressed = true;
                }
            }
        } else {
            rKeyWasPressed = false;
        }


        // 2. Then, handle the "isGameOver" transition
        if (ship != nullptr && ship->getIsDead()) {
            isGameOver = true;
        }

        if (isGameOver) {
            if (!isWaitingToEnd) {
                isWaitingToEnd = true;
                endGameTimer = 0.7f;
            }
            endGameTimer -= deltaTime;
            if (endGameTimer <= 0.0f) {
                currentState = GameState::GameOver;
                if (!gameOverSoundPlayed) {
                    StopMusic();
                    PlaySfx(gameOver, 0.8f);
                    gameOverSoundPlayed = true;
                }
                isWaitingToEnd = false;
                isGameOver = false;
            }
            return; // Now this return is fine because we already checked for the Reset key
        }


        // In Game::update()
        if (goalReached) {
            if (!isWaitingToSwitch) {
                isWaitingToSwitch = true;
                delayTimer = 2.0f; // 2 second delay
            }

            delayTimer -= deltaTime;

            if (delayTimer <= 0.0f) {
                int nextLevel = idToLoad + 1;

                // Check if next level exists
                if (levelFileRegistry.find(nextLevel) != levelFileRegistry.end()) {
                    requestSceneSwitch(nextLevel);
                } else {
                    // No more levels - show ending
                    currentState = GameState::Ending;
                    StopMusic();
                    PlayMusic(gameEndingMusic);

                    // play ending music or sound if you have one
                }

                isWaitingToSwitch = false;
                goalReached = false;
            }
        }



#ifdef _DEBUG

        // 1. Enter/Exit Input Mode
        if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS && !inputMode) {
            inputMode = true;
            levelInputBuffer = "";
            std::cout << "[DEBUG] Input Level ID (then press ENTER): " << std::flush;
        }

        if (inputMode) {
            // 2. Listen for keys 0-9 with a LATCH
            for (int i = 0; i <= 9; ++i) {
                int key = GLFW_KEY_0 + i;
                if (glfwGetKey(window, key) == GLFW_PRESS) {
                    if (!numKeysWasPressed[i]) {
                        levelInputBuffer += std::to_string(i);
                        std::cout << i << std::flush;
                        numKeysWasPressed[i] = true; // Set Latch
                    }
                } else {
                    numKeysWasPressed[i] = false; // Release Latch
                }
            }

            // 3. Confirm with ENTER
            if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
                if (!levelInputBuffer.empty()) {
                    int targetLevel = std::stoi(levelInputBuffer);
                    std::cout << "\n[GAME] Switching to Level: " << targetLevel << std::endl;

                    // --- ADD THIS LINE ---
                    this->idToLoad = targetLevel;
                    // ---------------------

                    this->switchScene(targetLevel);
                }
                inputMode = false;
                levelInputBuffer = "";
            }
        }
#endif
    }
    void Game::ShapesStartTouching(b2ShapeId shape_idA, b2ShapeId shape_idB) {
        Entity* entityA = static_cast<Entity*>(b2Shape_GetUserData(shape_idA));
        Entity* entityB = static_cast<Entity*>(b2Shape_GetUserData(shape_idB));

        if (entityA && entityB) {
            entityA->startContact(entityB);
            entityB->startContact(entityA);
        }
    }
    void Game::ShapesStopTouching(b2ShapeId shape_idA, b2ShapeId shape_idB) {
        Entity* entityA = static_cast<Entity*>(b2Shape_GetUserData(shape_idA));
        Entity* entityB = static_cast<Entity*>(b2Shape_GetUserData(shape_idB));

        if (entityA && entityB) {
            entityA->endContact(entityB);
            entityB->endContact(entityA);
        }
    }

    void Game::SensorStartsSensing(b2ShapeId sensorShapeId, b2ShapeId visitorShapeId) {
        void* sData = b2Shape_GetUserData(sensorShapeId);
        void* vData = b2Shape_GetUserData(visitorShapeId);

        //std::cout << "[HANDSHAKE] SensorData: " << sData << " | VisitorData: " << vData << std::endl;

        Entity* sensorEnt = static_cast<Entity*>(sData);
        Entity* visitorEnt = static_cast<Entity*>(vData);

        if (sensorEnt && visitorEnt) {
            //std::cout << "[HANDSHAKE] Calling onSensorEnter on Type: " << (int)sensorEnt->getType() << std::endl;
            sensorEnt->onSensorEnter(visitorEnt);
        }
    }

    void Game::SensorStopsSensing(b2ShapeId sensorShapeId, b2ShapeId visitorShapeId) {
        Entity* sensorEnt = static_cast<Entity*>(b2Shape_GetUserData(sensorShapeId));
        Entity* visitorEnt = static_cast<Entity*>(b2Shape_GetUserData(visitorShapeId));

        if (sensorEnt && visitorEnt) {
            sensorEnt->onSensorExit(visitorEnt);
        }
    }

    glm::mat4 Game::calculateUIMvPMatrix(glm::vec3 position, glm::vec3 scale) {
        // 1. Model Matrix (Positioned relative to center of screen)
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        model = glm::scale(model, scale);

        // 2. UI View Matrix (Identity - ignores camera position!)
        glm::mat4 view = glm::mat4(1.0f);

        float zoom = 2.0f;
        // 3. UI Projection Matrix (Simple Orthographic)
        glm::mat4 projection = glm::ortho(-zoom * aspect, zoom * aspect, -zoom, zoom, -1.0f, 1.0f);

        return projection * view * model;
    }

  void Game::draw() {

    if (currentState == GameState::MainMenu || currentState == GameState::Ending) {
        if (currentState == GameState::MainMenu) {
            if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
                int levelToStart = loadSavedLevel();
                if (levelToStart != -1) {
                    this->switchScene(levelToStart);
                    currentState = GameState::Playing;
                    PlayMusic(getMusicForLevel(levelToStart));
                }
            }

        }


        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        if (!spriteShader) {
            std::cout << "[DEBUG] Compiling spriteShader for Main Menu..." << std::endl;
            spriteShader = std::make_unique<gl3::Shader>("shaders/vertexShader.vert", "shaders/fragmentShader.frag");
        }

        std::string texturePath = (currentState == GameState::MainMenu)
                                  ? "assets/textures/main_menu.png"
                                  : "assets/textures/EndScreen.png"; // Load your Game Over texture here
        // IMPORTANT: Reload/Update the texture if the state changed
        static std::string lastPath = "";
        if (menuSprite == nullptr || lastPath != texturePath) {
            if (menuSprite) delete menuSprite; // Clean up old one
            GLuint menuTexID = Texture::LoadTexture(texturePath.c_str());
            menuSprite = new gl3::Sprite(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), menuTexID);
            lastPath = texturePath;
        }


        glm::vec3 menuPosition = glm::vec3(-0.05f, 0.0f, 0.0f);
        // Change -8.0f to 8.0f
        glm::vec3 menuScale = glm::vec3(7.5f, 4.0f, 1.0f);

        menuSprite->setPosition(menuPosition);
        menuSprite->setScale(menuScale);
        menuSprite->setColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

        spriteShader->use();
        spriteShader->setBool("useTexture", true);
        spriteShader->setInt("spriteTexture", 0);

        // Add 180 degrees to the rotation to flip the orientation without mirroring the matrix
        glm::mat4 menuMvp = calculateUIMvPMatrix(menuPosition, menuScale);
        menuSprite->draw(*spriteShader, menuMvp);

        glEnable(GL_DEPTH_TEST);
        glfwSwapBuffers(window);
    }
    else if (currentState == GameState::Paused || currentState == GameState::GameOver ) {
        // --- 1. DRAW THE FROZEN GAMEPLAY IN THE BACKGROUND ---
        glClear(GL_DEPTH_BUFFER_BIT); // Clear depth so UI renders over geometry
        glEnable(GL_DEPTH_TEST);
        if (currentScene) {
            currentScene->draw();
        }

        // --- 2. SET UP BLENDING FOR THE OVERLAY ---
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        if (!spriteShader) {
            spriteShader = std::make_unique<gl3::Shader>("shaders/vertexShader.vert", "shaders/fragmentShader.frag");
        }
        spriteShader->use();

        glm::vec3 backdropPos = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 uiScale = glm::vec3(8.0f, 4.0f, 1.0f);

        // --- 3. DRAW THE TRANSPARENT BLACK BACKDROP (PURE COLOR, NO IMAGE) ---
        spriteShader->setBool("useTexture", false);

        static gl3::Sprite* pauseBackdrop = nullptr;
        if (pauseBackdrop == nullptr) {
            pauseBackdrop = new gl3::Sprite(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 0);
        }

        pauseBackdrop->setPosition(backdropPos);
        pauseBackdrop->setScale(uiScale);
        pauseBackdrop->setColor(glm::vec4(0.0f, 0.0f, 0.0f, 0.6f));

        glm::mat4 backdropMvp = calculateUIMvPMatrix(backdropPos, uiScale);
        pauseBackdrop->draw(*spriteShader, backdropMvp);

        // --- 4. DRAW THE "PAUSED" TEXT SPRITE OVER THE BACKDROP ---
        spriteShader->setBool("useTexture", true);
        spriteShader->setInt("spriteTexture", 0);

        static gl3::Sprite* menuTextSprite = nullptr;
        std::string texturePath = (currentState == GameState::Paused)
                                  ? "assets/textures/pause_menu.png"
                                  : "assets/textures/over.png";
        // IMPORTANT: Reload/Update the texture if the state changed
        static std::string lastPath = "";
        if (menuTextSprite == nullptr || lastPath != texturePath) {
            if (menuTextSprite) delete menuTextSprite; // Clean up old one
            GLuint textTexID = Texture::LoadTexture(texturePath.c_str());
            menuTextSprite = new gl3::Sprite(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), textTexID);
            lastPath = texturePath;
        }


        glm::vec3 textPos = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 textScale = glm::vec3(7.0f, 4.0f, 1.0f);
        menuTextSprite->setPosition(textPos);
        menuTextSprite->setScale(textScale);
        menuTextSprite->setColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

        glm::mat4 textMvp = calculateUIMvPMatrix(textPos, textScale);
        menuTextSprite->draw(*spriteShader, textMvp);

        glUseProgram(0);
        glEnable(GL_DEPTH_TEST);
        glfwSwapBuffers(window);
    }
    else {
        // --- 5. NORMAL ACTIVE GAMEPLAY RENDERING BLOCK ---
        glClearColor(0.4f, 0.5f, 0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 1. DRAW THE OVERLAY FIRST (It now acts as a 'tint' on the background)
        // We draw this before the scene so it sits between the background and the entities
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        spriteShader->use();
        spriteShader->setBool("useTexture", false); // Solid color mode

        static gl3::Sprite* bgOverlay = nullptr;
        if (bgOverlay == nullptr) {
            // Use an existing valid texture ID if your Sprite class requires one, or 0 if it handles it
            bgOverlay = new gl3::Sprite(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(8.0f, 4.0f, 1.0f), 0);
        }

        bgOverlay->setColor(glm::vec4(0.0f, 0.0f, 0.0f, 0.55f));

        // Draw the overlay
        glm::mat4 overlayMvp = calculateUIMvPMatrix(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(8.0f, 4.0f, 1.0f));
        bgOverlay->draw(*spriteShader, overlayMvp);

        // 2. NOW DRAW THE SCENE (Entities rendered ON TOP of the darkness)
        glEnable(GL_DEPTH_TEST);
        if (currentScene) {
            currentScene->draw();
        }

        glfwSwapBuffers(window);

    }
}
    void Game::InitAudio() {
        audio.init();
        audio.setGlobalVolume(0.7f);

        menuMusic.load("assets/audio/caller.mp3");
        menuMusic.setLooping(true);

        // gameOverMusic.load("assets/audio/gameover.mp3");
        // gameOverMusic.setLooping(false);

        levelMusic.load("assets/audio/Factory.mp3");
        levelMusic.setLooping(true);

        levelMusic2.load("assets/audio/monoliths.mp3");
        levelMusic2.setLooping(true);

        levelMusic3.load("assets/audio/booya-b.mp3");
        levelMusic3.setLooping(true);

        gameEndingMusic.load("assets/audio/139973__jessepash__crowdyayapplause25ppllong.wav");

        openVent.load("assets/audio/metal_interaction1.wav");
        botStun.load("assets/audio/crackleelectricityloop.wav");
        flipSwitch.load("assets/audio/light_switch_turn_on.mp3");
        playerSpotted.load("assets/audio/441266__lilmati__hint-clue-trace-01.wav");
        boxPush_Pull.load("assets/audio/scrape-2.mp3");
        doorOpen_Close.load("assets/audio/scrape-4.wav");
        pipeRotated.load("assets/audio/metal_swing1.wav");
        laserFired.load("assets/audio/charge.wav");
        laserGuyHit.load("assets/audio/hit.wav");
        pipeLand.load("assets/audio/708061__whi1ter1ce__dropping-pipe-on-hard-floor.wav");
        dogWalk.load("assets/audio/761559__stuniverso__crab-walking.wav");
        pickUpStick.load("assets/audio/metal_swing1.wav");
        gameOver.load("assets/audio/760731__buffsatan__misson-failed.mp3");
    }

    void Game::PlayMusic(SoLoud::WavStream& music, float volume) {
        if (currentMusicHandle != 0) {
            audio.stop(currentMusicHandle);
        }
        currentMusicHandle = audio.playBackground(music, 0.8f);
    }
    void Game::StopMusic() {
        audio.stop(currentMusicHandle);
        currentMusicHandle = 0;
    }

    void Game::StopSFX(SoLoud::handle handle) {
        audio.fadeVolume(handle, 0.0f, 0.3f); // fade to 0 over 0.3 seconds
        audio.scheduleStop(handle, 0.3f);      // then stop
    }


    void Game::PlaySfx(SoLoud::Wav& sfx, float volume) {
        audio.play(sfx, volume); // fire and forget, no handle needed
    }
    SoLoud::handle Game::PlaySfxWithHandle(SoLoud::Wav& sfx, float volume) {
        return audio.play(sfx, volume);
    }
    SoLoud::WavStream& Game::getMusicForLevel(int levelID) {
        if (levelID >= 1 && levelID <= 5) return levelMusic;
        if (levelID >= 6 && levelID <= 10) return levelMusic2;
        return levelMusic3; // 11-15
    }

    void Game::updateDeltaTime() {
        float frameTime = glfwGetTime();
        deltaTime = frameTime - lastFrameTime;
        lastFrameTime = frameTime;
    }

    void Game::requestSceneSwitch(int ID) {
        pendingLevelID = ID;
        levelLoaded = false; // Signal that the current level state is dirty and needs reload
        std::cout << "Switch requested for ID: " << ID << std::endl;
    }
    void Game::switchScene(int ID) {
        isResetting = true; // Blockieren AN
        goalReached = false;


        auto it = levelFileRegistry.find(ID);
        if (it == levelFileRegistry.end()) {
            isResetting = false; // Nicht vergessen, bei Fehler zu entsperren!
            return;
        }

        // 1. Logische Verbindung trennen
        this->setShip(nullptr);

        // 2. Physics-Events leeren, BEVOR wir die Objekte zerstören!
        // Damit Box2D die Events vergisst, die sich auf die sterbenden Objekte beziehen.
        b2World_GetContactEvents(physicsWorld);
        b2World_GetSensorEvents(physicsWorld);

        // 3. Szene und Entities aufräumen
        if (currentScene) {
            currentScene->getEntities().clear();
        }
        currentScene.reset();

        // 4. Jetzt erst die Welt zerstören
        b2DestroyWorld(physicsWorld);
        physicsWorld = b2CreateWorld(&worldDef);
        SoLoud::WavStream& newMusic = getMusicForLevel(ID);
        SoLoud::WavStream& currentMusic = getMusicForLevel(idToLoad); // idToLoad is still the OLD level here

        if (&newMusic != &currentMusic) {
            PlayMusic(newMusic);
        }

        currentScene = std::make_unique<gl3::Scene>(this);
        currentScene->setPhysicsWorld(physicsWorld);
        currentScene->loadLevelData(it->second, physicsWorld);

        this->idToLoad = ID;
        isResetting = false; // Blockieren AUS
    }

    const b2WorldId Game::getPhysicsWorld() const {
        return physicsWorld;
    }

    const b2Vec2 Game::getGravity() const {
        return worldDef.gravity;
    }


    void Game::resetGame() {
        isGameOver = false; // Turn off the death screen
        gameOverSoundPlayed = false;

        // This wipes everything and reloads from the text file
        switchScene(idToLoad);
        currentState = GameState::Playing;

        std::cout << "[GAME] Level Hard Reset!" << std::endl;
        PlayMusic(getMusicForLevel(idToLoad));
        // isResetting = false;
    }

    bool Game::isShipValid() {
        // 1. Ist er Null?
        if (ship == nullptr) return false;

        // // 2. Ist das Objekt selbst schon am sterben/löschen?
        // // Hier kannst du ein Flag in deiner Entity-Klasse nutzen:
        // if (ship->isMarkedForDeletion()) return false;

        return true;
    }


    void Game::saveGame() {
        // 1. Define the directory path (e.g., a "saves" folder in your project)
        fs::path saveDir = "saveData";

        // 2. Create the directory if it doesn't exist
        if (!fs::exists(saveDir)) {
            fs::create_directories(saveDir);
        }

        // 3. Define the full file path
        fs::path filePath = saveDir / "savegame.txt";

        std::ofstream saveFile(filePath);
        if (saveFile.is_open()) {
            saveFile << idToLoad;
            saveFile.close();
            std::cout << "[GAME] Saved to: " << filePath << std::endl;
        }
    }

    int Game::loadSavedLevel() {
        fs::path filePath = fs::path("saveData") / "savegame.txt";

        if (!fs::exists(filePath)) {
            return -1; // Default to Level 1 if no save exists
        }

        std::ifstream saveFile(filePath);
        int level = 1;
        if (saveFile.is_open()) {
            saveFile >> level;
            saveFile.close();
        }
        return level;
    }


    }


