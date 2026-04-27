#if 0
#include "level2.h"
#include <iostream>

#include "../Game.h"
#include "box2d/box2d.h"
#include "../entities/Dog.h"
#include "../entities/Floor.h"
#include "../entities/Box.h"
#include "../entities/Door.h"
#include "../entities/SecBot.h"
#include "../entities/Stick.h"
#include "../entities/Switch.h"
#include "../entities/Ship.h" //technically scene could do this --> there is no level without player

namespace gl3 {
    void level2::load(b2WorldId physicsWorld) {
        Scene::load(PhysicsWorld);
        this->levelID = 2;
        //MOVE ALL OBJECT LOADING CODE TO RESPECTIVE LEVELS
        spriteShader = std::make_unique<gl3::Shader>(
           "shaders/vertexShader.vert",
           "shaders/fragmentShader.frag");
        // Create a much wider floor for continuous side-scrolling
        // Position floor at y = 0 (will be at bottom with camera at y=1.0)
        float floorWidth = 100.0f; // Very wide floor
        float floorHeight = 0.1f;   // Thin floor

        auto firstFloor = std::make_unique<Floor>( spriteShader.get(),
                   glm::vec3(0.0f, -0.99f, 0.0f),           // Center at y=0
                   glm::vec3(floorWidth, floorHeight, 1.0f), // Wide and thin
                   glm::vec4(0.5f, 0.3f, 0.1f, 1.0f),     // Brown color for ground
                   PhysicsWorld, "assets/textures/floor1.png"
               );

        auto secondFloor = std::make_unique<Floor>( spriteShader.get(),
            glm::vec3(0.0f, 1.2f, 0.0f),           // Center at y=0
            glm::vec3(floorWidth, floorHeight, 1.0f), // Wide and thin
            glm::vec4(0.5f, 0.3f, 0.1f, 1.0f),     // Brown color for ground
            PhysicsWorld, "assets/textures/floor2.png"
        );




        std::cout << "=== FLOOR DEBUG ===" << std::endl;
        std::cout << "Floor position: " << firstFloor->getPosition().y << std::endl;
        std::cout << "Floor half-height: " << firstFloor->GetHalfHeight() << std::endl;
        std::cout << "Floor top Y: " << (firstFloor->GetFloorY() + firstFloor->GetHalfHeight()) << std::endl;

        // Create box on the floor
        float floorTop = firstFloor->GetFloorY() + firstFloor->GetHalfHeight();

        // ADD A SMALL BUFFER (e.g., 0.01f)
        float spawnBuffer = 0.01f;
        // Box scale is the full width/height, Box class divides by 2 internally
        // So scale.y = 0.25f means halfHeight = 0.125f
        float boxScale = 0.6f;
        float boxHalfHeight = boxScale / 2.0f;  // This matches what Box constructor does

        auto box = std::make_unique<Box>( spriteShader.get(),
            glm::vec3(1.0f, floorTop + boxHalfHeight, 0.0f),
            glm::vec3(boxScale, boxScale, 0.0f),
            glm::vec4(0.8f, 0.2f, 0.2f, 1.0f),
            PhysicsWorld
        );

        auto floorSwitchA = std::make_unique<Switch>( spriteShader.get(),
            glm::vec3(0.0f, floorTop, 0.0f),
            glm::vec3(0.5f,0.1f, 0.0f),
            glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
            PhysicsWorld
            );


        // 1. Calculate the necessary scale (diameter in Box2D meters)
        const float botRadius = 0.5f;
        const float botDiameter = 1.0f;          // 3.5 meters
        auto secBot = std::make_unique<SecBot>( spriteShader.get(),
            glm::vec3(2.0f, floorTop + botRadius, 0.0f),
            glm::vec3(botDiameter,botDiameter, 0.0f),
            glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
            PhysicsWorld
            );
        auto doorA = std::make_unique<Door>( spriteShader.get(),
            glm::vec3(3.0f, floorTop + boxHalfHeight + 0.5f, 0.0f),
            glm::vec3(0.4f,2.0f, 0.0f),
            glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
            PhysicsWorld
            );
        floorSwitchA->setTargetDoor(doorA.get());


        // Create ship at temporary position to get its half-height
        auto spaceShip = std::make_unique<Ship>( spriteShader.get(),
            glm::vec3(-1.0f, 0.5f, 0.0f),
            glm::vec3(.6f, 1.3f, 0.0f),
            PhysicsWorld
        );

        auto stick = std::make_unique<Stick>( spriteShader.get(),
            glm::vec3(-1.5f, 0.0f, 0.0f),
            glm::vec3(0.3f, 0.08f, 0.0f),
            PhysicsWorld);




        auto dog = std::make_unique<Dog>( spriteShader.get(),
            glm::vec3(-1.0f, 0.0f, 0.0f),
            glm::vec3(0.5f, 0.5f, 0.0f),
            PhysicsWorld);
        std::cout << "\n=== SHIP DEBUG ===" << std::endl;
        std::cout << "Ship half-height: " << spaceShip->playerHalfHeight << std::endl;

        b2BodyId body = dog->getBody();
        b2Vec2 position = b2Body_GetPosition(body);
        std::cout << "Physical Dog Y: " << position.y << " | Floor Y: " << floorTop << " | Gap: " << (position.y - floorTop) << std::endl;


        // Calculate correct spawn position (just above floor)
        float correctY = floorTop + spaceShip->playerHalfHeight + spawnBuffer;

        std::cout << "Calculated spawn Y: " << correctY << std::endl;

        // Set the ship position
        spaceShip->setPosition(glm::vec3(-3.0f, correctY, 0.0f));

        std::cout << "\n=== AFTER POSITIONING ===" << std::endl;
        glm::vec3 pos = spaceShip->getPosition();
        std::cout << "Ship render position: (" << pos.x << ", " << pos.y << ")" << std::endl;

        b2Vec2 bodyPos = b2Body_GetPosition(spaceShip->getBody());
        std::cout << "Ship physics position: (" << bodyPos.x << ", " << bodyPos.y << ")" << std::endl;

        spaceShip->setStick(stick.get());
        game->setShip(spaceShip.get());
        stick->setDawg(dog.get());
        std::cout << stick->getDawg() << " <-- this is dog " << std::endl;
        // --- SETUP LAYERS ---
        // You can even add/subtract from the enum for fine-tuning
        firstFloor->setZOrder(Layers::BACKGROUND);
        secondFloor->setZOrder(Layers::BACKGROUND);
        floorSwitchA->setZOrder(Layers::MIDGROUND);
        doorA->setZOrder(Layers::MIDGROUND);
        box->setZOrder(Layers::FOREGROUND);     // Box is in front of the switch
        secBot->setZOrder(Layers::FOREGROUND);
        spaceShip->setZOrder(Layers::PLAYER);   // Player is on top of the ground
        stick->setZOrder(Layers::UI);           // Dots are always on the very top
        dog->setZOrder(Layers::PLAYER);


        entities.push_back(std::move(firstFloor));
        entities.push_back(std::move(secondFloor));
        entities.push_back(std::move(floorSwitchA));
        entities.push_back(std::move(doorA));
        entities.push_back(std::move(spaceShip));
        entities.push_back(std::move(box));
        entities.push_back(std::move(secBot));
        entities.push_back(std::move(stick));
        entities.push_back(std::move(dog));



    }

}
#endif
