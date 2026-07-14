#include "../Game.h"
#include "Scene.h"
#include "../entities/entities.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>

#include "../entities/LaserGuy.h"

namespace gl3 {
    void Scene::load(b2WorldId physicsWorld) {
        //MOVE ALL OBJECT LOADING CODE TO RESPECTIVE LEVELS
        spriteShader = std::make_unique<gl3::Shader>(
            "shaders/vertexShader.vert",
            "shaders/fragmentShader.frag");
        // Pass 'Scene::MyPreSolve' and 'this'
        b2World_SetPreSolveCallback(physicsWorld, Scene::MyPreSolve, this);
    }

    void Scene::loadLevelData(const std::string &levelName, b2WorldId world) {
        entities.clear();
        allVents.clear();

        load(world);

        std::cout << "Working Directory: " << std::filesystem::current_path() << std::endl;
        std::ifstream file(levelName);
        if (!file.is_open()) {
            std::cout << "ERROR: Could not open level file at " << levelName << std::endl;
            return;
        }

        std::map<int, Switch *> switchMap;
        std::map<int, Door *> doorMap;
        std::map<int, Speaker *> speakerMap;
        std::map<int, MirrorCube *> mirrorCubeMap;
        std::map<int, SpeakerSwitch *> speakerSwitchMap;
        std::map<int, PipeSwitch *> pipeSwitchMap;

        std::string line;
        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '#') continue;

            std::stringstream ss(line);
            std::string type;
            float x, y, sx, sy, rotation;
            int linkID;
            bool isGround;
            bool is_Exit;
            int floorID;


            if (ss >> type >> x >> y >> sx >> sy >> rotation >> linkID >> isGround >> is_Exit >> floorID) {
                glm::vec3 pos(x, y, 0.0f);
                glm::vec3 scale(sx, sy, 1.0f);
                // Attempt to read floorID, but if it's not there, default to 0
                // int floorID = 0;
                // ss >> floorID;


                if (type == "Ship") {
                    entities.push_back(std::make_unique<Ship>(spriteShader.get(), pos, scale, floorID, world));

                } else if (type == "Box") {
                    entities.push_back(std::make_unique<Box>(spriteShader.get(), pos, scale, world));
                } else if (type == "Floor") {
                    // int groundFlag = 1;
                    // ss >> groundFlag;
                    // bool isGround = (groundFlag == 1);
                    entities.push_back(std::make_unique<Floor>(spriteShader.get(), pos, scale, world, isGround,floorID));
                } else if (type == "Wall") {
                    entities.push_back(std::make_unique<Wall>(spriteShader.get(), pos, scale, world));
                } else if (type == "Door") {
                    auto door = std::make_unique<Door>(spriteShader.get(), pos, scale, world);
                    if (linkID != 0) doorMap[linkID] = door.get();
                    entities.push_back(std::move(door));
                } else if (type == "Switch") {
                    auto sw = std::make_unique<Switch>(spriteShader.get(), pos, scale, floorID, world);
                    if (linkID != 0) switchMap[linkID] = sw.get();
                    entities.push_back(std::move(sw));
                } else if (type == "SecBot") {
                    entities.push_back(std::make_unique<SecBot>(spriteShader.get(), pos, scale,rotation, floorID, world));
                }else if (type=="LaserGuy") {
                    entities.push_back(std::make_unique<LaserGuy>(spriteShader.get(), pos, scale, world, rotation));
                }
                else if (type == "Mirror") {
                    entities.push_back(std::make_unique<Mirror>(spriteShader.get(), pos, scale, rotation, linkID, world));
                }
                else if (type == "MirrorCube") {
                    std::cout << "DEBUG: Loading MirrorCube with rotation: " << rotation << std::endl;
                    auto cube = std::make_unique<MirrorCube>(spriteShader.get(), pos, scale, rotation,linkID, floorID,world);
                    if (linkID != 0) mirrorCubeMap[linkID] = cube.get(); // You are doing this!
                    entities.push_back(std::move(cube));
                }
                else if (type == "PipeSwitch") {
                    auto pipeSwitch = std::make_unique<PipeSwitch>(spriteShader.get(), pos, scale, world);
                    if (linkID != 0) pipeSwitchMap[linkID] = pipeSwitch.get(); // <--- YOU ARE MISSING THIS!
                    entities.push_back(std::move(pipeSwitch));
                }
                else if (type == "Dog") {
                    entities.push_back(std::make_unique<Dog>(spriteShader.get(), pos, scale, floorID, world));
                } else if (type == "Stick") {
                    entities.push_back(std::make_unique<Stick>(spriteShader.get(), pos, scale, floorID, world));
                } else if (type == "Ladder") {
                    entities.push_back(std::make_unique<Ladder>(spriteShader.get(), pos, scale, world));
                    // Inside the parsing loop in Scene.cpp
                } else if (type == "Speaker") {
                    auto spk = std::make_unique<Speaker>(spriteShader.get(), pos, scale, world);
                    if (linkID != 0) speakerMap[linkID] = spk.get(); // MUST ADD THIS LINE
                    entities.push_back(std::move(spk));
                } else if (type == "SpeakerSwitch") {
                    auto spw = std::make_unique<SpeakerSwitch>(spriteShader.get(), pos, scale, world);
                    if (linkID != 0) speakerSwitchMap[linkID] = spw.get(); // MUST ADD THIS LINE
                    entities.push_back(std::move(spw));
                }
                else if (type == "Vent") {
                    entities.push_back(std::make_unique<Vent>(spriteShader.get(), pos, scale, world, linkID, is_Exit));

                    // 2. Use dynamic_cast for safe runtime type checking
                    Vent* ventPtr = dynamic_cast<Vent*>(entities.back().get());
                    if (ventPtr != nullptr) {
                        allVents.push_back(ventPtr);
                    }
                } else if (type == "TutorialSign") {
                    entities.push_back(std::make_unique<TutorialSign>(spriteShader.get(), pos, scale,linkID, world));

                }
                else if (type == "GoalSensor") {
                    entities.push_back(std::make_unique<GoalSensor>(spriteShader.get(), pos, scale, world));
                }
            }else {
                // FAILURE
                std::cout << "[LOADER ERROR] Failed to parse line: " << line << std::endl;
            }

        }

        // Find the dog first
        Dog *levelDog = nullptr;
        for (auto &entity : entities) {
            if (entity->getType() == EntityType::Dog) {
                levelDog = static_cast<Dog *>(entity.get());
                break;
            }
        }
        for(auto* v : allVents) {
            if (v == nullptr) {
                std::cout << "[CRITICAL] Found a NULL vent in registry!" << std::endl;
            }
        }

        // In Scene.cpp, after loading entities:
        for (auto* ventA : allVents) {
            if (!ventA->getIsExit()) { // We are looking at an Entrance
                for (auto* ventB : allVents) {
                    if (ventB->getIsExit() && ventA->getVentId() == ventB->getVentId()) {
                        ventA->initConnectedTwin(ventB);
                        std::cout << "[DEBUG] Linking Entry ID " << ventA->getVentId()
                                  << " (" << ventA << ") to Exit " << ventB << std::endl;
                    }
                }
            }
        }
        // Now, use your already populated 'allVents' vector
        if (levelDog) {
            for (Vent* v : allVents) {
                if (!v->getIsExit()) {
                    levelDog->setEntryVent(v);
                    // Since we linked the twins earlier in the function,
                    // the dog's exVent is now automatically the twin!
                    levelDog->setExitVent(v->getConnectedTwin());
                    std::cout << "[LINKER] Dog linked to Entry: " << v << " and Exit: " << v->getConnectedTwin() << std::endl;
                }
            }
        }
        // Now print your verification cleanly once everything settles!
        std::cout << "\n--- POST-LOAD VENT STATUS VERIFICATION ---" << std::endl;
        for (auto* vent : allVents) {
            std::cout << "Vent Memory Address: " << vent
                      << " | ID: " << vent->getVentId()
                      << " | IsOpen Status: " << vent->getIsOpen() << std::endl;
        }
        std::cout << "------------------------------------------\n" << std::endl;

        // --- LINKING SWITCHES AND DOORS ---
        for (auto const &[id, swPtr]: switchMap) {
            if (doorMap.find(id) != doorMap.end()) {
                std::cout << "[DEBUG] Linking Switch " << id << " to Door... " << std::flush;
                swPtr->setTargetDoor(doorMap[id]);
                std::cout << "OK" << std::endl;
            }
        }

        // --- LINKING SPEAKER SWITCHES AND SPEAKERS ---
        for (auto const &[id, swPtr]: speakerSwitchMap) {
            if (speakerMap.find(id) != speakerMap.end()) {
                std::cout << "[DEBUG] Linking SpeakerSwitch " << id << " to Speaker... ";
                swPtr->setTargetSpeaker(speakerMap[id]); // Use Speaker pointer, not Door
                std::cout << "OK" << std::endl;
            }
        }

        // --- LINKING SPEAKER SWITCHES AND SPEAKERS ---
        for (auto const &[id, swPtr]: pipeSwitchMap) {
            if (mirrorCubeMap.find(id) != mirrorCubeMap.end()) {
                std::cout << "[DEBUG] Linking SpeakerSwitch " << id << " to Speaker... ";
                swPtr->setTargetCube(mirrorCubeMap[id]); // Use Speaker pointer, not Door
                std::cout << "OK" << std::endl;
            }
        }

        // --- SAFE SORTING ---
        std::cout << "[DEBUG] Starting Stable Sort... " << std::flush;
        std::stable_sort(entities.begin(), entities.end(),
                         [](const std::unique_ptr<Entity> &a, const std::unique_ptr<Entity> &b) {
                             if (!a || !b) return false;
                             return a->getZOrder() < b->getZOrder();
                         });
        std::cout << "OK" << std::endl;

        // --- ENTITY HANDSHAKE FENCING ---
        Ship *playerShip = nullptr;
        Stick *playerStick = nullptr;

        Speaker *speaker = nullptr;
        SpeakerSwitch *speakerSwitch = nullptr;
        SecBot *secBot = nullptr;


        std::cout << "[DEBUG] Searching for handshake entities... " << std::flush;
        for (auto &entity: entities) {
            if (!entity) continue;

            if (entity->getType() == EntityType::Player) playerShip = static_cast<Ship *>(entity.get());
            if (entity->getType() == EntityType::Stick) playerStick = static_cast<Stick *>(entity.get());
            if (entity->getType() == EntityType::Dog) levelDog = static_cast<Dog *>(entity.get());
            if (entity->getType() == EntityType::Speaker) speaker = static_cast<Speaker *>(entity.get());
            if (entity->getType() == EntityType::SpeakerSwitch)
                speakerSwitch = static_cast<SpeakerSwitch *>(entity.get());
            if (entity->getType() == EntityType::SecBot) secBot = static_cast<SecBot *>(entity.get());

            game->setShip(playerShip);

        }
        // 3. Now perform the handshakes for all bots
        for (auto &entity : entities) {
            if (entity->getType() == EntityType::SecBot) {
                SecBot* bot = static_cast<SecBot*>(entity.get());
                if (playerShip) {
                    bot->setShip(playerShip); // Assign to EVERY bot
                }
            }
        }
        std::cout << "Found (Ship: " << (playerShip ? "YES" : "NO")
                << ", Stick: " << (playerStick ? "YES" : "NO")
                << ", Dog: " << (levelDog ? "YES" : "NO") << ")" << std::endl;

        // The Critical Handshake
        if (playerShip && playerStick) {
            std::cout << "[CRITICAL] Calling Ship::setStick... " << std::flush;
            playerShip->setStick(playerStick);
            std::cout << "SUCCESS" << std::endl;
        }


        if (playerStick && levelDog) {
            std::cout << "[CRITICAL] Calling Stick::setDawg... " << std::flush;
            playerStick->setDawg(levelDog);
            std::cout << "SUCCESS" << std::endl;
        }


        doorMap.clear();
        switchMap.clear();
        mirrorCubeMap.clear();
        pipeSwitchMap.clear();


        std::cout << "[DEBUG] loadLevelData FINISHED SUCCESSFULLY" << std::endl;

    }


    void Scene::update(float dt) {
        for (auto &entity: entities) {
            entity->update(game, dt);
        }
        // 2. Remove any entities marked for death
        entities.erase(
            std::remove_if(entities.begin(), entities.end(),
                [](const std::unique_ptr<Entity>& e) {
                    // Check if this is a LaserGuy and if he is marked for removal
                    auto* lg = dynamic_cast<LaserGuy*>(e.get());
                    return lg != nullptr && lg->getShouldBeRemoved();
                }),
            entities.end()
        );
    }

    void Scene::updatePhysics(float dt) {
        const float fixedTimeStep = 1.0f / 60.0f;
        const int subStepCount = 4;
        accumulator += dt;

        while (accumulator >= fixedTimeStep) {
            // 1. Step the world
            b2World_Step(game->getPhysicsWorld(), fixedTimeStep, subStepCount);
            if (game->getIsResetting()) {
                accumulator = 0.0f; // drain remaining time so next frame starts fresh
                return;
            }

            // 2. FETCH EVENTS IMMEDIATELY AFTER STEP
            b2ContactEvents contactEvents = b2World_GetContactEvents(game->getPhysicsWorld());

            // 3. DO THE HANDSHAKE HERE
            for (int i = 0; i < contactEvents.beginCount; ++i) {
                b2ContactBeginTouchEvent *beginEvent = contactEvents.beginEvents + i;
                // Access your Game's helper or do it directly:
                if (!b2Shape_IsValid(beginEvent->shapeIdA)) continue;
                if (!b2Shape_IsValid(beginEvent->shapeIdB)) continue;
                game->ShapesStartTouching(beginEvent->shapeIdA, beginEvent->shapeIdB);
            }

            // Handle end contacts too so activePlayer becomes null when you walk away
            for (int i = 0; i < contactEvents.endCount; ++i) {
                b2ContactEndTouchEvent *endEvent = contactEvents.endEvents + i;
                if (!b2Shape_IsValid(endEvent->shapeIdA)) continue;
                if (!b2Shape_IsValid(endEvent->shapeIdB)) continue;
                game->ShapesStopTouching(endEvent->shapeIdA, endEvent->shapeIdB);
            }

            b2SensorEvents sensorEvents = b2World_GetSensorEvents(game->getPhysicsWorld());


            for (int i = 0; i < sensorEvents.beginCount; ++i) {
                std::cout << "ARE WE EVEN DOING ANYTHING????" << std::endl;
                b2SensorBeginTouchEvent *beginEvent = sensorEvents.beginEvents + i;
                if (!b2Shape_IsValid(beginEvent->sensorShapeId)) continue;
                if (!b2Shape_IsValid(beginEvent->visitorShapeId)) continue;

                std::cout << "BEFORE CALLING SENSORSSTARTSENSING" << std::endl;
                std::cout << sensorEvents.beginCount << std::endl;
                game->SensorStartsSensing(beginEvent->sensorShapeId, beginEvent->visitorShapeId);
            }

            for (int i = 0; i < sensorEvents.endCount; ++i) {
                b2SensorEndTouchEvent *endEvent = sensorEvents.endEvents + i;
                if (!b2Shape_IsValid(endEvent->sensorShapeId)) continue;
                if (!b2Shape_IsValid(endEvent->visitorShapeId)) continue;
                game->SensorStopsSensing(endEvent->sensorShapeId, endEvent->visitorShapeId);

            }


            accumulator -= fixedTimeStep;
        }

        for (auto &entity: entities) {
            entity->updateBasedOnPhysics();
        }
    }

    void Scene::draw() {
        if (!spriteShader) return;

        spriteShader->use();
        spriteShader->setBool("useTexture", true);

        for (auto &entity: entities) {
            // This now calls the fixed version in Entity.cpp
            entity->draw(game);
        }
    }

    bool Scene::MyPreSolve(b2ShapeId shapeIdA, b2ShapeId shapeIdB, b2Vec2 point, b2Vec2 normal, void *context) {
        // 1. Get our scene context and the entities involved
        Scene *myScene = static_cast<Scene *>(context);
        Entity *entA = (Entity *) b2Shape_GetUserData(shapeIdA);
        Entity *entB = (Entity *) b2Shape_GetUserData(shapeIdB);

        if (!entA || !entB || !myScene) return true;

        // 2. Identify the Player and the Floor
        Ship *player = nullptr;
        Floor *floor = nullptr;

        if (entA->getType() == EntityType::Player) {
            player = static_cast<Ship *>(entA);
            if (entB->getType() == EntityType::Floor) floor = static_cast<Floor *>(entB);
        } else if (entB->getType() == EntityType::Player) {
            player = static_cast<Ship *>(entB);
            if (entA->getType() == EntityType::Floor) floor = static_cast<Floor *>(entA);
        }

        if (player && floor) {
            if (floor->getIsGround()) return true;

            if (player->getIsClimbing()) return false;

        }

        // Default: Be solid
        return true;
    }

    void Scene::setDeltatime() {
        deltatime = game->getDeltaTime();
    }

    Scene::~Scene() {

        entities.clear();
    }
}
