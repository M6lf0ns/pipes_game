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
#include "entities/Floor.h"
#include "glm/gtx/string_cast.hpp"
#include "rendering/Texture.h"
#include "rendering/Shader.h"
#include "entities/Floor.h"

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
        gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
        if (glGetError() != GL_NO_ERROR) {
            throw std::runtime_error("gl error");
        }

        audio.init();
        audio.setGlobalVolume(0.1f);

        // Create the physics world
        b2WorldDef worldDef = b2DefaultWorldDef();
        // We use worldDef to define our physics world
        worldDef.gravity = b2Vec2{0.0f, -9.8f};
        physicsWorld = b2CreateWorld(&worldDef);
    }

    Game::~Game() {
        glfwTerminate();
    }

    glm::mat4 Game::calculateMvpMatrix(glm::vec3 position, float zRotationInDegrees, glm::vec3 scale) {
        // Model matrix
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        model = glm::rotate(model, glm::radians(zRotationInDegrees), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, scale);

        // No need for a complex view; identity works for a static 2D camera
        glm::mat4 view = glm::mat4(1.0f);

        glm::mat4 projection = glm::ortho(-aspect, aspect, -1.0f, 1.0f, -1.0f, 1.0f);
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


        auto floor = std::make_unique<Floor>(
            glm::vec3(0.0f, -1.0f + 0.05f, 0.0f), // center X at 0, place near bottom (y=-1 + halfHeight)
            glm::vec3(aspect, 0.05f, 1.0f), // full width (halfwidth=aspect), thin height
            glm::vec4(1, 0, 0, 1),
            physicsWorld
        );
        std::cout << "=== FLOOR DEBUG ===" << std::endl;
        std::cout << "Floor position: " << floor->getPosition().y << std::endl;
        std::cout << "Floor half-height: " << floor->GetHalfHeight() << std::endl;
        std::cout << "Floor top Y: " << (floor->GetFloorY() + floor->GetHalfHeight()) << std::endl;

        auto box = std::make_unique<Box>(glm::vec3(0.0f, 0.3f, 0.0f), glm::vec3(0.25f, 0.25f, 0.0f), glm::vec4(0.0f),
                                         physicsWorld);
        // Create ship at temporary position to get its half-height
        auto spaceShip = std::make_unique<Ship>(glm::vec3(0.25f, 0.25f, 0.0f),
                                                0.0f, glm::vec3(0.25f, 0.25f, 0.25f), physicsWorld);

        std::cout << "\n=== SHIP DEBUG (at y=0) ===" << std::endl;
        std::cout << "Ship half-height: " << spaceShip->shipHalfHeight << std::endl;
        b2AABB shipAABB = b2Shape_GetAABB(spaceShip->shape);
        std::cout << "Ship AABB at y=0: minY=" << shipAABB.lowerBound.y
                << ", maxY=" << shipAABB.upperBound.y << std::endl;

        // Calculate correct spawn position
        float floorTop = floor->GetFloorY() + floor->GetHalfHeight();
        float correctY = floorTop + spaceShip->shipHalfHeight;
        float boxCorrectY = floorTop + box->GetHalfHeight();

        std::cout << "\n=== SPAWN CALCULATION ===" << std::endl;
        std::cout << "Calculated correctY: " << correctY << std::endl;

        // Set the position
        spaceShip->setPosition(glm::vec3(0.5f, correctY, 0.0f));
        //box->setPosition(glm::vec3(0.2f, boxCorrectY, 0.0f));


        std::cout << "\n=== AFTER setPosition ===" << std::endl;
        glm::vec3 pos = spaceShip->getPosition();
        std::cout << "Ship render position: (" << pos.x << ", " << pos.y << ", " << pos.z << ")" << std::endl;

        b2Vec2 bodyPos = b2Body_GetPosition(spaceShip->getBody());
        std::cout << "Ship physics body position: (" << bodyPos.x << ", " << bodyPos.y << ")" << std::endl;

        shipAABB = b2Shape_GetAABB(spaceShip->shape);
        std::cout << "Ship AABB after reposition: minY=" << shipAABB.lowerBound.y
                << ", maxY=" << shipAABB.upperBound.y << std::endl;
        std::cout << "Ship bottom should be at: " << shipAABB.lowerBound.y
                << " | Floor top is at: " << floorTop << std::endl;

        ship = spaceShip.get();
        entities.push_back(std::move(floor));
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
        const int subStepCount = 4; // recommended sub-step count
        accumulator += deltaTime;
        if (accumulator >= fixedTimeStep) {
            b2World_Step(physicsWorld, fixedTimeStep, subStepCount);
            ContactListener::checkForCollision(physicsWorld);

            // Update the entities based on what happened in the physics step
            for (const std::unique_ptr<Entity> &entity: entities) {
                entity->updateBasedOnPhysics();
            }
            accumulator -= fixedTimeStep;
        }
    }

    const b2WorldId Game::getPhysicsWorld() const {
        return physicsWorld;
    }
}
