#define GLM_ENABLE_EXPERIMENTAL
#include "Game.h"
#include <stdexcept>
#include <random>
#include <iostream>
#include "entities/Enemy.h"
#include "Assets.h"
#include "physics/ContactListener.h"
#include "box2d/box2d.h"
#include "entities/Box.h"
#include "entities/Door.h"
#include "entities/Floor.h"
#include "glm/gtx/string_cast.hpp"
#include "rendering/Texture.h"
#include "rendering/Shader.h"
#include "entities/Floor.h"
#include "entities/SecBot.h"
#include "entities/Stick.h"
#include "entities/Switch.h"
#include <algorithm> //required for sort

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

        audio.init();
        audio.setGlobalVolume(0.1f);

        // Create the physics world
        worldDef = b2DefaultWorldDef();
        worldDef.gravity = b2Vec2{0.0f, -9.8f};
        physicsWorld = b2CreateWorld(&worldDef);

        // Instead of storing unique_ptr functions, just store the filename
        levelFileRegistry[1] = "../../assets/levels/level1.txt";
        levelFileRegistry[2] = "../../assets/levels/level2.txt";
        levelFileRegistry[3] = "../../assets/levels/level3.txt";
        levelFileRegistry[4] = "../../assets/levels/level4.txt";
        levelFileRegistry[5] = "../../assets/levels/level5.txt";
        levelFileRegistry[6] = "../../assets/levels/level6.txt";
        levelFileRegistry[7] = "../../assets/levels/level7.txt";

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

        // 2. View Matrix
        float shipX = (ship != nullptr) ? ship->getPosition().x : 0.0f;
        float cameraY = 1.0f;
        glm::mat4 view = glm::lookAt(
            glm::vec3(shipX, cameraY, 1.0f),
            glm::vec3(shipX, cameraY, 0.0f),
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

            if (isGameOver) {
                if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
                    resetGame();
                }
                glfwPollEvents();
                continue;
            }

            update();

            // Only update the level scenes if we are actively playing
            if (currentState == GameState::Playing && currentScene) {
                currentScene->update(deltaTime);
                currentScene->updatePhysics(deltaTime);
            }

            // ALWAYS DRAW! Move this outside the scene check block
            draw();

            // --- THE SAFE ZONE ---
            if (pendingLevelID != -1 && !levelLoaded) {
                switchScene(pendingLevelID);
                idToLoad = pendingLevelID;
                pendingLevelID = -1;
            }

            glfwPollEvents();
        }

        glDeleteVertexArrays(1, &VAO);
    }

    void Game::update() {
        bool justPressedESC = glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS;

        if (justPressedESC) {
            // ONLY trigger the switch on the FIRST frame the key goes down
            if (!ESCWasPressed) {
                if (currentState == GameState::MainMenu || currentState == GameState::Paused) {
                    std::cout << "[GAME] Exiting from Main Menu!" << std::endl;
                    glfwSetWindowShouldClose(window, true);
                    return;
                }
                ESCWasPressed = true; // Lock the latch instantly


                if (currentState == GameState::Playing) {
                    std::cout << "[GAME] Paused!" << std::endl;
                    currentState = GameState::Paused;
                    return; // Stop processing this frame completely
                }
            }
        } else {
            ESCWasPressed = false; // Reset the latch only when they let go
        }

        // --- STATE GUARDS ---
        if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
        if (currentState == GameState::MainMenu || currentState == GameState::Paused) {
                std::cout << "[GAME] Starting game from Main Menu!" << std::endl;
                currentState = GameState::Playing;
            return;
            }

        }



        if (isGameOver) {
            if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
                resetGame();
            }
            return;
        }
        // In Game::update()
        if (goalReached) {
            if (!isWaitingToSwitch) {
                isWaitingToSwitch = true;
                delayTimer = 2.0f; // 2 second delay
            }

            delayTimer -= deltaTime;

            if (delayTimer <= 0.0f) {
                // ONLY NOW do we set the ID and trigger the switch
                int nextLevel = idToLoad + 1;
                requestSceneSwitch(nextLevel);

                // Reset flags
                isWaitingToSwitch = false;
                goalReached = false;
            }
        }




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
                    this->switchScene(targetLevel);
                }
                inputMode = false;
                levelInputBuffer = "";
            }
        }
    }
    void Game::ShapesStartTouching(b2ShapeId shape_idA, b2ShapeId shape_idB) {
        Entity* entityA = static_cast<Entity*>(b2Shape_GetUserData(shape_idA));
        Entity* entityB = static_cast<Entity*>(b2Shape_GetUserData(shape_idB));

        if (entityA && entityB) {
            // "A, you hit B" and "B, you hit A"
            entityA->startContact(entityB);
            entityB->startContact(entityA);
        }
    }
    void Game::ShapesStopTouching(b2ShapeId shape_idA, b2ShapeId shape_idB) {
        Entity* entityA = static_cast<Entity*>(b2Shape_GetUserData(shape_idA));
        Entity* entityB = static_cast<Entity*>(b2Shape_GetUserData(shape_idB));

        if (entityA && entityB) {
            // "A, you hit B" and "B, you hit A"
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


  void Game::draw() {
    if (currentState == GameState::MainMenu) {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        if (!spriteShader) {
            std::cout << "[DEBUG] Compiling spriteShader for Main Menu..." << std::endl;
            spriteShader = std::make_unique<gl3::Shader>("shaders/vertexShader.vert", "shaders/fragmentShader.frag");
        }

        if (menuSprite == nullptr) {
            std::cout << "[DEBUG] Loading Main Menu Texture..." << std::endl;
            GLuint menuTexID = Texture::LoadTexture("assets/textures/start_menu.png");
            menuSprite = new gl3::Sprite(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), menuTexID);
        }

        glm::vec3 menuPosition = glm::vec3(0.0f, 1.0f, 0.0f);
        // Change -8.0f to 8.0f
        glm::vec3 menuScale = glm::vec3(8.0f, 4.0f, 1.0f);

        menuSprite->setPosition(menuPosition);
        menuSprite->setScale(menuScale);
        menuSprite->setColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

        spriteShader->use();
        spriteShader->setBool("useTexture", true);
        spriteShader->setInt("spriteTexture", 0);

        // Add 180 degrees to the rotation to flip the orientation without mirroring the matrix
        glm::mat4 menuMvp = calculateMvpMatrix(menuPosition, 0.0f, menuScale);
        menuSprite->draw(*spriteShader, menuMvp);

        glEnable(GL_DEPTH_TEST);
        glfwSwapBuffers(window);
    }
    else if (currentState == GameState::Paused) {
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

        float cameraX = (ship != nullptr) ? ship->getPosition().x : 0.0f;
        glm::vec3 dynamicUIPos = glm::vec3(cameraX, 1.0f, 0.0f);
        glm::vec3 uiScale = glm::vec3(8.0f, 4.0f, 1.0f);

        // --- 3. DRAW THE TRANSPARENT BLACK BACKDROP (PURE COLOR, NO IMAGE) ---
        spriteShader->setBool("useTexture", false);

        static gl3::Sprite* pauseBackdrop = nullptr;
        if (pauseBackdrop == nullptr) {
            pauseBackdrop = new gl3::Sprite(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 0);
        }

        pauseBackdrop->setPosition(dynamicUIPos);
        pauseBackdrop->setScale(uiScale);
        pauseBackdrop->setColor(glm::vec4(0.0f, 0.0f, 0.0f, 0.6f));

        glm::mat4 backdropMvp = calculateMvpMatrix(dynamicUIPos, 0.0f, uiScale);
        pauseBackdrop->draw(*spriteShader, backdropMvp);

        // --- 4. DRAW THE "PAUSED" TEXT SPRITE OVER THE BACKDROP ---
        spriteShader->setBool("useTexture", true);
        spriteShader->setInt("spriteTexture", 0);

        static gl3::Sprite* pauseTextSprite = nullptr;
        if (pauseTextSprite == nullptr) {
            std::cout << "[DEBUG] Loading Pause Text Texture File..." << std::endl;
            GLuint textTexID = Texture::LoadTexture("assets/textures/pause_menu.png");
            pauseTextSprite = new gl3::Sprite(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), textTexID);
        }

        glm::vec3 textScale = glm::vec3(7.0f, 4.0f, 1.0f);
        pauseTextSprite->setPosition(dynamicUIPos);
        pauseTextSprite->setScale(textScale);
        pauseTextSprite->setColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

        glm::mat4 textMvp = calculateMvpMatrix(dynamicUIPos, 0.0f, textScale);
        pauseTextSprite->draw(*spriteShader, textMvp);

        glUseProgram(0);
        glEnable(GL_DEPTH_TEST);
        glfwSwapBuffers(window);
    }
    else {
        // --- 5. NORMAL ACTIVE GAMEPLAY RENDERING BLOCK ---
        // Clear old frame buffers entirely so menu imagery completely vanishes!
        glClearColor(0.172f, 0.243f, 0.313f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(0);
        glEnable(GL_DEPTH_TEST);

        if (currentScene) {
            currentScene->draw();
        }

        glfwSwapBuffers(window);
    }
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
        std::cout << "[DEBUG] SWITCHING SCENE TO: " << ID << std::endl;
        goalReached = false;
        auto it = levelFileRegistry.find(ID);
        if (it == levelFileRegistry.end()) return;

        // --- CRITICAL CLEANUP ---
        if (currentScene) {
            // Ensure every entity knows its physics world is dead
            // This stops them from trying to access it in their destructors
            currentScene->getEntities().clear();
        }
        // 1. Wipe the old scene and physics world
        currentScene.reset();
        this->setShip(nullptr);

        b2World_GetContactEvents(physicsWorld);
        b2World_GetSensorEvents(physicsWorld);
        b2DestroyWorld(physicsWorld);
        physicsWorld = b2CreateWorld(&worldDef);

        // 2. Create a generic Scene object
        currentScene = std::make_unique<gl3::Scene>(this);

        // 3. Set the world and LOAD FROM FILE
        currentScene->setPhysicsWorld(physicsWorld);

        // This is the NEW line that replaces the old load()
        currentScene->loadLevelData(it->second, physicsWorld);

    }

    const b2WorldId Game::getPhysicsWorld() const {
        return physicsWorld;
    }

    const b2Vec2 Game::getGravity() const {
        return worldDef.gravity;
    }

    void Game::resetGame() {
        isGameOver = false;

        if (ship) {
            ship->isDead = false;
            b2Rot rotation = b2MakeRot(0.0f);
            getShip()->setPosition(glm::vec3(1.5f, 0.0f, 0.0f));
            b2Body_SetLinearVelocity(ship->getBody(), {0.0f, 0.0f});
            b2Body_SetAngularVelocity(ship->getBody(), 0.0f);
            b2Body_Enable(ship->getBody());
            std::cout << "Game Reset!" << std::endl;
            gameOverTimer = 0.0f;
        }
    }


    }


