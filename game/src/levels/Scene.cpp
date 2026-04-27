#include "../Game.h"
#include "Scene.h"
#include "../entities/entities.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
namespace gl3 {

    void Scene::load(b2WorldId physicsWorld) {

        //MOVE ALL OBJECT LOADING CODE TO RESPECTIVE LEVELS
        spriteShader = std::make_unique<gl3::Shader>(
           "shaders/vertexShader.vert",
           "shaders/fragmentShader.frag");


    }
    void Scene::loadLevelData(const std::string& levelName, b2WorldId world) {
        load(world);

        // Use the variable passed from Game.cpp instead of a hardcoded string
        std::ifstream file(levelName);

        if (!file.is_open()) {
            std::cout << "ERROR: Could not open level file at " << levelName << std::endl;
            return;
        }

        // Temporary maps to store Switch and Door pointers for linking
        std::map<int, Switch*> switchMap;
        std::map<int, Door*> doorMap;

        std::string line;
        while (std::getline(file, line)) {
            // Skip empty lines or comments starting with #
            if (line.empty() || line[0] == '#') continue;

            std::stringstream ss(line);
            std::string type;
            float x, y, sx, sy;
            int linkID;

            // Extract values from the line
            if (ss >> type >> x >> y >> sx >> sy >> linkID) {
                glm::vec3 pos(x, y, 0.0f);
                glm::vec3 scale(sx, sy, 1.0f);

                if (type == "Ship") {
                    entities.push_back(std::make_unique<Ship>(spriteShader.get(), pos, scale, world));
                }
                else if (type == "Box") {
                    entities.push_back(std::make_unique<Box>(spriteShader.get(), pos, scale, world));
                }
                else if (type == "Floor") {
                    entities.push_back(std::make_unique<Floor>(spriteShader.get(), pos, scale,  world));
                }
                else if (type == "Door") {
                    auto door = std::make_unique<Door>(spriteShader.get(), pos, scale, world);
                    if (linkID != 0) doorMap[linkID] = door.get();
                    entities.push_back(std::move(door));
                }
                else if (type == "Switch") {
                    auto sw = std::make_unique<Switch>(spriteShader.get(), pos, scale, world);
                    if (linkID != 0) switchMap[linkID] = sw.get();
                    entities.push_back(std::move(sw));
                }
                else if (type == "SecBot") { // <--- ADD THIS
                    entities.push_back(std::make_unique<SecBot>(spriteShader.get(), pos, scale, world));
                    std::cout << "Successfully spawned SecBot at " << x << ", " << y << std::endl;
                }
                else if (type == "Dog") { // <--- ADD THIS
                    entities.push_back(std::make_unique<Dog>(spriteShader.get(), pos, scale, world));
                    std::cout << "Successfully spawned Dog at " << x << ", " << y << std::endl;
                }
                else if (type == "Stick") { // <--- ADD THIS
                    entities.push_back(std::make_unique<Stick>(spriteShader.get(), pos, scale, world));
                    std::cout << "Successfully spawned Stick at " << x << ", " << y << std::endl;
                }
                else if (type == "Ladder") { // <--- ADD THIS
                    entities.push_back(std::make_unique<Ladder>(spriteShader.get(), pos, scale, world));
                    std::cout << "Successfully spawned Stick at " << x << ", " << y << std::endl;
                }

            }
        }

        // matches the door of ID "_" to the current switch of the same ID
        for (auto const& [id, swPtr] : switchMap) {
            if (doorMap.find(id) != doorMap.end()) {
                swPtr->setTargetDoor(doorMap[id]);
                std::cout << "Linked Switch ID " << id << " to Door ID " << id << std::endl;
            }
        }


        std::stable_sort(entities.begin(), entities.end(),
        [](const std::unique_ptr<Entity>& a, const std::unique_ptr<Entity>& b) {
            return a->getZOrder() < b->getZOrder();
        }

        );

        // 1. Find the three "actors"
        Ship* playerShip = nullptr;
        Stick* playerStick = nullptr;
        Dog* levelDog = nullptr;

        // At the very end of Scene::loadLevelData
        for (auto& entity : entities) {
            if (entity->getType() == EntityType::Player) playerShip = static_cast<Ship*>(entity.get());
            if (entity->getType() == EntityType::Stick)  playerStick = static_cast<Stick*>(entity.get());
            if (entity->getType() == EntityType::Dog)    levelDog = static_cast<Dog*>(entity.get());
        }

        // 2. The Handshake (Only if they all exist in this level)
        if (playerShip && playerStick) {
            playerShip->setStick(playerStick);
        }

        if (playerStick && levelDog) {
            playerStick->setDawg(levelDog);
        }


    }


    void Scene::update(float dt) {

        for (auto &entity: entities) {
            entity->update(game, dt);
        }

    }
    void Scene::updatePhysics(float dt) {
        const float fixedTimeStep = 1.0f / 60.0f;
        const int subStepCount = 4;
        accumulator += dt;

        while (accumulator >= fixedTimeStep) {
            // 1. Step the world
            b2World_Step(game->getPhysicsWorld(), fixedTimeStep, subStepCount);

            // 2. FETCH EVENTS IMMEDIATELY AFTER STEP
            b2ContactEvents contactEvents = b2World_GetContactEvents(game->getPhysicsWorld());

            // 3. DO THE HANDSHAKE HERE
            for (int i = 0; i < contactEvents.beginCount; ++i) {
                b2ContactBeginTouchEvent* beginEvent = contactEvents.beginEvents + i;
                // Access your Game's helper or do it directly:
                game->ShapesStartTouching(beginEvent->shapeIdA, beginEvent->shapeIdB);
            }

            // Handle end contacts too so activePlayer becomes null when you walk away
            for (int i = 0; i < contactEvents.endCount; ++i) {
                b2ContactEndTouchEvent* endEvent = contactEvents.endEvents + i;
                game->ShapesStopTouching(endEvent->shapeIdA, endEvent->shapeIdB);
            }

            b2SensorEvents sensorEvents = b2World_GetSensorEvents(game->getPhysicsWorld());


            for (int i = 0; i < sensorEvents.beginCount; ++i) {

                std::cout << "ARE WE EVEN DOING ANYTHING????" << std::endl;
                b2SensorBeginTouchEvent* beginEvent = sensorEvents.beginEvents + i;

                std::cout << "BEFORE CALLING SENSORSSTARTSENSING" << std::endl;
                std::cout << sensorEvents.beginCount << std::endl;
                game->SensorStartsSensing(beginEvent->sensorShapeId, beginEvent->visitorShapeId);

            }

            for (int i = 0; i < sensorEvents.endCount; ++i) {

                b2SensorEndTouchEvent* endEvent = sensorEvents.endEvents + i;
                game->SensorStopsSensing(endEvent->sensorShapeId, endEvent->visitorShapeId);
            }


            accumulator -= fixedTimeStep;
        }

        for (auto& entity : entities) {
            entity->updateBasedOnPhysics();
        }
    }
    void Scene::draw() {
        if (!spriteShader) return;

        spriteShader->use();
        spriteShader->setBool("useTexture", true);

        for (auto &entity : entities) {
            // This now calls the fixed version in Entity.cpp
            entity->draw(game);
        }
    }







    void Scene::setDeltatime() {
        deltatime = game->getDeltaTime();
    }

    Scene::~Scene() {
        entities.clear();

    }

}