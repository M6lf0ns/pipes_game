#define GLM_ENABLE_EXPERIMENTAL
#include "Game.h"
#include <stdexcept>
#include <random>
#include <iostream>
#include "entities/Planet.h"
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
#include "entities/Switch.h"

namespace gl3 {



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
        b2WorldDef worldDef = b2DefaultWorldDef();
        worldDef.gravity = b2Vec2{0.0f, -9.8f};
        physicsWorld = b2CreateWorld(&worldDef);
    }

    Game::~Game() {
        glfwTerminate();
    }

    glm::mat4 Game::calculateMvpMatrix(glm::vec3 position, float zRotation, glm::vec3 scale) {
        // Model matrix
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        model = glm::rotate(model, glm::radians(zRotation), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, scale);

        // Camera follows player horizontally but stays at a fixed vertical position
        // This keeps the floor at the bottom of the screen
        float cameraY = 1.0f; // Fixed camera height (adjust as needed)
        glm::mat4 view = glm::lookAt(
            glm::vec3(ship->getPosition().x, cameraY, 1.0f),
            glm::vec3(ship->getPosition().x, cameraY, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f)
        );

        // Orthographic projection for 2D side-scroller
        float zoom = 2.0f;
        glm::mat4 projection = glm::ortho(
            -zoom * aspect,  // left
            zoom * aspect,   // right
            -zoom,           // bottom (this is where floor should be visible)
            zoom,            // top
            0.1f,
            100.0f
        );

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

        // Create a much wider floor for continuous side-scrolling
        // Position floor at y = 0 (will be at bottom with camera at y=1.0)
        float floorWidth = 100.0f; // Very wide floor
        float floorHeight = 0.1f;   // Thin floor

        auto floor = std::make_unique<Floor>(
            glm::vec3(0.0f, 0.0f, 0.0f),           // Center at y=0
            glm::vec3(floorWidth, floorHeight, 1.0f), // Wide and thin
            glm::vec4(0.5f, 0.3f, 0.1f, 1.0f),     // Brown color for ground
            physicsWorld
        );

        std::cout << "=== FLOOR DEBUG ===" << std::endl;
        std::cout << "Floor position: " << floor->getPosition().y << std::endl;
        std::cout << "Floor half-height: " << floor->GetHalfHeight() << std::endl;
        std::cout << "Floor top Y: " << (floor->GetFloorY() + floor->GetHalfHeight()) << std::endl;

        // Create box on the floor
        float floorTop = floor->GetFloorY() + floor->GetHalfHeight();
        // Box scale is the full width/height, Box class divides by 2 internally
        // So scale.y = 0.25f means halfHeight = 0.125f
        float boxScale = 0.25f;
        float boxHalfHeight = boxScale / 2.0f;  // This matches what Box constructor does

        auto box = std::make_unique<Box>(
            glm::vec3(2.0f, floorTop + boxHalfHeight, 0.0f),
            glm::vec3(boxScale, boxScale, 0.0f),
            glm::vec4(0.8f, 0.2f, 0.2f, 1.0f),
            physicsWorld
        );

        auto floorSwitch = std::make_unique<Switch>(
            glm::vec3(1.0f, floorTop, 0.0f),
            glm::vec3(0.3f,0.07f, 0.0f),
            glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
            physicsWorld
            );

        auto door = std::make_unique<Door>(glm::vec3(4.0f, floorTop + boxHalfHeight, 0.0f),
            glm::vec3(0.25f,1.1f, 0.0f),
            glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
            physicsWorld
            );

        doorptr = door.get(); //store pointer before move

        // Create ship at temporary position to get its half-height
        auto spaceShip = std::make_unique<Ship>(
            glm::vec3(0.0f, 0.25f, 0.0f),
            0.0f,
            glm::vec3(0.25f, 0.25f, 0.25f),
            physicsWorld
        );

        std::cout << "\n=== SHIP DEBUG ===" << std::endl;
        std::cout << "Ship half-height: " << spaceShip->shipHalfHeight << std::endl;

        // Calculate correct spawn position (just above floor)
        float correctY = floorTop + spaceShip->shipHalfHeight;

        std::cout << "Calculated spawn Y: " << correctY << std::endl;

        // Set the ship position
        spaceShip->setPosition(glm::vec3(0.0f, correctY, 0.0f));

        std::cout << "\n=== AFTER POSITIONING ===" << std::endl;
        glm::vec3 pos = spaceShip->getPosition();
        std::cout << "Ship render position: (" << pos.x << ", " << pos.y << ")" << std::endl;

        b2Vec2 bodyPos = b2Body_GetPosition(spaceShip->getBody());
        std::cout << "Ship physics position: (" << bodyPos.x << ", " << bodyPos.y << ")" << std::endl;

        ship = spaceShip.get();
        entities.push_back(std::move(floor));
        entities.push_back(std::move(floorSwitch));
        entities.push_back(std::move(door));
        entities.push_back(std::move(spaceShip));
        entities.push_back(std::move(box));


        backgroundMusic = std::make_unique<SoLoud::Wav>();
        backgroundMusic->load(resolveAssetPath("audio/electronic-wave.mp3").string().c_str());
        backgroundMusic->setLooping(true);
        audio.playBackground(*backgroundMusic);

        glfwSetTime(1.0 / 60);

        spriteShader = std::make_unique<gl3::Shader>(
            "shaders/vertexShader.vert",
            "shaders/fragmentShader.frag");


        while (!glfwWindowShouldClose(window)) {
            update();
            updatePhysics();
            draw();
            updateDeltaTime();
            glfwPollEvents();
        }

        glDeleteVertexArrays(1, &VAO);
    }

    void Game::update() {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }

        for (auto &entity: entities) {
            entity->update(this, deltaTime);
        }

        b2ContactEvents contactEvents = b2World_GetContactEvents(physicsWorld);


        for (int i = 0; i < contactEvents.beginCount; ++i) {
            b2ContactBeginTouchEvent* beginEvent = contactEvents.beginEvents + i;
            ShapesStartTouching(beginEvent->shapeIdA, beginEvent->shapeIdB);
            //std::cout << contactEvents.beginCount << std::endl;
        }

        for (int i = 0; i < contactEvents.endCount; ++i) {
            b2ContactEndTouchEvent* endEvent = contactEvents.endEvents + i;
            if (b2Shape_IsValid(endEvent->shapeIdA) && b2Shape_IsValid(endEvent->shapeIdB)) {
                ShapesStopTouching(endEvent->shapeIdA, endEvent->shapeIdB);
            }
        }
        b2SensorEvents sensorEvents = b2World_GetSensorEvents(physicsWorld);


        for (int i = 0; i < sensorEvents.beginCount; ++i) {

            std::cout << "ARE WE EVEN DOING ANYTHING????" << std::endl;
            b2SensorBeginTouchEvent* beginEvent = sensorEvents.beginEvents + i;
            std::cout << "BEFORE CALLING SENSORSSTARTSENSING" << std::endl;
            std::cout << sensorEvents.beginCount << std::endl;
            SensorStartsSensing(beginEvent->sensorShapeId, beginEvent->visitorShapeId);

        }

        for (int i = 0; i < sensorEvents.endCount; ++i) {

            b2SensorEndTouchEvent* endEvent = sensorEvents.endEvents + i;
            SensorStopsSensing(endEvent->sensorShapeId, endEvent->visitorShapeId);
        }
    }

    void Game::ShapesStartTouching(b2ShapeId shape_idA, b2ShapeId shape_idB) {
        if (!b2Shape_IsValid(shape_idA) || !b2Shape_IsValid(shape_idB)) {
            return;
        }

        void* userDataA = b2Shape_GetUserData(shape_idA);
        std::cout << "User Data A: " << userDataA << std::endl;
        void* userDataB = b2Shape_GetUserData(shape_idB);
        std::cout << "User Data B: " << userDataB << std::endl;

        if (!userDataA || !userDataB) {
            return;
        }

        Entity* entityA = static_cast<Entity*>(userDataA);
        Entity* entityB = static_cast<Entity*>(userDataB);

        Box* box = nullptr;
        if (entityA->getType() == EntityType::Player &&
            entityB->getType() == EntityType::Box) {
            box = static_cast<Box*>(entityB);

        }
        else if (entityA->getType() == EntityType::Box &&
                 entityB->getType() == EntityType::Player) {
            box = static_cast<Box*>(entityA);

            std::cout << "Start Contact function called " << std::endl;
        }
        if (box) {
            box->startContact();
        }


    }

    void Game::ShapesStopTouching(b2ShapeId shape_idA, b2ShapeId shape_idB) {
        if (!b2Shape_IsValid(shape_idA) || !b2Shape_IsValid(shape_idB)) {
            return;
        }

        void* userDataA = b2Shape_GetUserData(shape_idA);
        void* userDataB = b2Shape_GetUserData(shape_idB);

        if (!userDataA || !userDataB) {
            return;
        }

        Entity* entityA = static_cast<Entity*>(userDataA);
        Entity* entityB = static_cast<Entity*>(userDataB);

        Box* box = nullptr;
        if (entityA->getType() == EntityType::Player &&
            entityB->getType() == EntityType::Box) {
            box = static_cast<Box*>(entityB);

            }
        else if (entityA->getType() == EntityType::Box &&
                 entityB->getType() == EntityType::Player) {
            box = static_cast<Box*>(entityA);

            std::cout << "End Contact function called " << std::endl;
                 }
        if (box) {
            box->endContact();
        }



    }
    void Game::SensorStartsSensing(b2ShapeId sensorShapeId, b2ShapeId visitorShapeId) {
        if (!b2Shape_IsValid(sensorShapeId) || !b2Shape_IsValid(visitorShapeId)) {
            return;
        }


        void* sensorUserData = b2Shape_GetUserData(sensorShapeId);
        void* visitorUserData = b2Shape_GetUserData(visitorShapeId);

        if (!sensorUserData || !visitorUserData) {
            std::cout << "Invalid sensor user data!" << std::endl;
            return;
        }

        Entity* sensorEntity = static_cast<Entity*>(sensorUserData);
        Entity* visitorEntity = static_cast<Entity*>(visitorUserData);

        if (sensorEntity->getType() == EntityType::Switch)  {
            Switch* switchEntity = static_cast<Switch*>(sensorEntity);

            if (b2Body_GetType(visitorEntity->getBody()) == b2_dynamicBody) {
                if (visitorEntity->getType() == EntityType::Box) {
                    boxOnPlateCount += 1;
                }else if (visitorEntity->getType() == EntityType::Player) {
                    playerOnPlateCount += 1;
                }
                if (boxOnPlateCount >= 1 || playerOnPlateCount >= 1) {
                    doorptr->openDoor();
                }

                std::cout << "Dynamic body has come in contact with switch" << std::endl;
            }
        }



    }

    void Game::SensorStopsSensing(b2ShapeId sensorShapeId, b2ShapeId visitorShapeId) {
        if (!b2Shape_IsValid(sensorShapeId) || !b2Shape_IsValid(visitorShapeId)) {
            return;
        }

        void* sensorUserData = b2Shape_GetUserData(sensorShapeId);
        void* visitorUserData = b2Shape_GetUserData(visitorShapeId);

        if (!sensorUserData || !visitorUserData) {
            return;
        }

        Entity* sensorEntity = static_cast<Entity*>(sensorUserData);
        Entity* visitorEntity = static_cast<Entity*>(visitorUserData);

        if (sensorEntity->getType() == EntityType::Switch)  {
            Switch* switchEntity = static_cast<Switch*>(sensorEntity);

            //in order to avoid door closing when player gets off pressure plate, we check if anyone´s still on there
            if (b2Body_GetType(visitorEntity->getBody()) == b2_dynamicBody) {
                if (visitorEntity->getType() == EntityType::Box) {
                    boxOnPlateCount -= 1;
                }else if (visitorEntity->getType() == EntityType::Player) {
                    playerOnPlateCount -= 1;
                }
                if ((boxOnPlateCount < 1 && playerOnPlateCount < 1 )) {
                    doorptr->closeDoor();
                }

                std::cout << "Dynamic body has come in contact with switch" << std::endl;
            }
        }


    }

    void Game::draw() {
        glClearColor(0.172f, 0.243f, 0.313f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        for (auto &entity: entities) {
            entity->draw(this);
        }

        glfwSwapBuffers(window);
    }

    void Game::updateDeltaTime() {
        float frameTime = glfwGetTime();
        deltaTime = frameTime - lastFrameTime;
        lastFrameTime = frameTime;
    }
    void Game::updatePhysics() {
        const float fixedTimeStep = 1.0f / 60.0f;
        const int subStepCount = 4;
        accumulator += deltaTime;
        if (accumulator >= fixedTimeStep) {
            b2World_Step(physicsWorld, fixedTimeStep, subStepCount);
            ContactListener::checkForCollision(physicsWorld);

            // DEBUG: Identify each entity
            int entityCount = 0;
            for (const std::unique_ptr<Entity> &entity: entities) {
                // std::cout << "Physics updating entity " << entityCount++
                //           << " type: " << static_cast<int>(entity->getType()) << std::endl;
                entity->updateBasedOnPhysics();
            }
            accumulator -= fixedTimeStep;
        }
    }

    const b2WorldId Game::getPhysicsWorld() const {
        return physicsWorld;
    }
}
