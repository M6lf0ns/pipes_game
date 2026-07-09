#include "SecBot.h"
#include <iostream>
#include "../Assets.h"
#include "../Game.h"
#include "../rendering/Texture.h"
#include "box2d/box2d.h"
#include "box2d/types.h"
#include "../PhysicsCategories.h"
#include "../physics/PhysicsUtils.h"
#include "box2d/src/sensor.h"


namespace gl3 {
    SecBot::SecBot(Shader* shader, glm::vec3 position, glm::vec3 scale,float rotation, int id, b2WorldId physicsWorld) : Entity (
        shader,
        position,
        scale,
        physicsWorld,
        EntityType::SecBot)
    ,  floorID(id), rotationAng(rotation)

    {
        this->sprite = new Sprite(position, scale, Texture::LoadTexture("assets/textures/secBot.png"));
        this->setZOrder(3);
        SecBot::createPhysicsBody();
        this->currentState = Botstate::patrol; // Force initial state
        this->isTurning = false;
        this->lastKnownX = 0.0f;
        std::cout << "[DEBUG] SecBot created at address: " << this << std::endl;

        this->patrolFrames.push_back(Texture::LoadTexture("assets/textures/bot_patrol_1.png"));
        this->patrolFrames.push_back(Texture::LoadTexture("assets/textures/bot_patrol_2.png"));
        this->patrolFrames.push_back(Texture::LoadTexture("assets/textures/bot_patrol_3.png"));
        this->patrolFrames.push_back(Texture::LoadTexture("assets/textures/bot_patrol_4.png"));
        this->patrolFrames.push_back(Texture::LoadTexture("assets/textures/bot_patrol_5.png"));

        this->chaseFrames.push_back(Texture::LoadTexture("assets/textures/bot_chase_1.png"));
        this->chaseFrames.push_back(Texture::LoadTexture("assets/textures/bot_chase_2.png"));
        this->chaseFrames.push_back(Texture::LoadTexture("assets/textures/bot_chase_3.png"));
        this->chaseFrames.push_back(Texture::LoadTexture("assets/textures/bot_chase_4.png"));
        this->chaseFrames.push_back(Texture::LoadTexture("assets/textures/bot_chase_5.png"));

        this->searchFrames.push_back(Texture::LoadTexture("assets/textures/bot_search_1.png"));
        this->searchFrames.push_back(Texture::LoadTexture("assets/textures/bot_search_2.png"));
        this->searchFrames.push_back(Texture::LoadTexture("assets/textures/bot_search_3.png"));
        this->searchFrames.push_back(Texture::LoadTexture("assets/textures/bot_search_4.png"));
        this->searchFrames.push_back(Texture::LoadTexture("assets/textures/bot_search_5.png"));



    }


    void SecBot::update(Game *game, float deltaTime) {
        if (game->getIsGameOver() || isFrozen || !ship) {
            std::cout << "SECBOT CANT FIND SHIP" << std::endl;
            return;
        }

        animTimer += deltaTime;
        if (animTimer >= 0.03f) { // Your frame speed
            animTimer = 0.0f;
            if (currentState == Botstate::patrol) {
                animFrame = (animFrame + 1) % patrolFrames.size();
                this->sprite->setTexture(patrolFrames[animFrame]);

            }
            else if (currentState == Botstate::chase) {
                animFrame = (animFrame + 1) % chaseFrames.size();
                this->sprite->setTexture(chaseFrames[animFrame]);

            } else if (currentState == Botstate::search) {
                animFrame = (animFrame + 1) % searchFrames.size();
                this->sprite->setTexture(searchFrames[animFrame]);
            }


        }

        Entity::update(game, deltaTime);



        float dist = glm::distance(ship->getPosition(), this->getPosition());
        // if (currentState == Botstate::chase && dist < 0.3f) {
        //     ship->kill(game);
        //
        // } COMMENT BACK IN LATER
        if (currentState == Botstate::patrol && isTurning) {
            timeRemainingBeforeTurn -= deltaTime;
            b2Body_SetLinearVelocity(body, {0.0f, 0.0f});
            if (timeRemainingBeforeTurn <= 0.0f) flipSpriteDirection();
            return;
        }

        castRay();
        checkForGroundAhead();



        // Only chase if on the same floor AND spotted
        if (((playerSpotted && !isViewBlocked) || ((dist <= 1.5f)) && canReachPlayer())) {
            currentState = Botstate::chase;
            lastKnownX = ship->getPosition().x;
        } else if (currentState == Botstate::chase) {
            if (isViewBlocked || !canReachPlayer() || !playerSpotted) {
                currentState = Botstate::search; // Lost them
                forgetTimer = 0.0f;
            }

        }
        bool isGroundAhead = checkForGroundAhead();




        // std::cout << "[DEBUG] Current State: " << (int)currentState
        //           << " | Spotted: " << playerSpotted
        // << " | View Blocked: " << isViewBlocked
        //           << " | CanReach: " << canReachPlayer() << std::endl;
        // --- MOVEMENT LOGIC ---
        if (currentState == Botstate::chase) {
            float dir = (ship->getPosition().x < position.x) ? -1.0f : 1.0f;
            patrolDirection = dir;
            sprite->setScale({dir * abs(sprite->getScale().x), sprite->getScale().y, sprite->getScale().z});

            float velErr = ((PATROL_SPEED * 1.5f) * dir) - b2Body_GetLinearVelocity(body).x;
            // Constant speed (1.5x patrol speed)
            b2Body_SetLinearVelocity(body, {dir * (PATROL_SPEED * 1.5f), b2Body_GetLinearVelocity(body).y});
        }
        else if (currentState == Botstate::patrol) {
            if (isGroundAhead) {
                flipSpriteDirection();
            }
            float velErr = (PATROL_SPEED * patrolDirection) - b2Body_GetLinearVelocity(body).x;
            b2Body_SetLinearVelocity(body, {patrolDirection * PATROL_SPEED, b2Body_GetLinearVelocity(body).y});
        }
        else if (currentState == Botstate::search) {
            forgetTimer += deltaTime;
            // Move toward the saved lastKnownPosition
            float dir = (lastKnownX < position.x) ? -1.0f : 1.0f;

            // Only move if we aren't already at the location
            if (std::abs(position.x - lastKnownX) > 0.2f && !isGroundAhead) {
                patrolDirection = dir;
                sprite->setScale({dir * abs(sprite->getScale().x), sprite->getScale().y, sprite->getScale().z});

                float velErr = (PATROL_SPEED * dir) - b2Body_GetLinearVelocity(body).x;
                b2Body_SetLinearVelocity(body, {dir * PATROL_SPEED, b2Body_GetLinearVelocity(body).y});
            } else {
                b2Body_SetLinearVelocity(body, {0.0f, b2Body_GetLinearVelocity(body).y});
            }

            if (forgetTimer >= 3.0f) {
                currentState = Botstate::patrol;
                forgetTimer = 0.0f;
            }
        }
    }
    void SecBot::createPhysicsBody() {

        // 1. Calculate the Radius (0.5m)
        botRadius = scale.x/2;

        // 2. Define and Create Body
        b2BodyDef botBodyDef = b2DefaultBodyDef();
        botBodyDef.type = b2_dynamicBody;
        botBodyDef.position = {position.x, position.y};
        botBodyDef.userData = this;
        body = b2CreateBody(physicsWorld, &botBodyDef);

        // 3. Define the Shape Geometry
        // *** Safer Initialization using struct assignment (C++11/14) ***
        circleGeometry = { {0.0f, -0.1f}, botRadius };
        // This explicitly initializes center to {0,0} and radius.

        // 4. Define the Shape Properties
        b2ShapeDef botShapeDef = b2DefaultShapeDef();
        botShapeDef.enableContactEvents = true;
        botShapeDef.density = 1.0f;
        botShapeDef.userData = this; // Must be set before creation
        botShapeDef.filter.categoryBits = gl3::PhysicsCategories::CATEGORY_SEC_BOT;
        botShapeDef.filter.maskBits = gl3::PhysicsCategories::CATEGORY_FLOOR |
                                  gl3::PhysicsCategories::CATEGORY_WALL |
                                  gl3::PhysicsCategories::CATEGORY_BOX |
                                      gl3::PhysicsCategories::CATEGORY_DOOR |
                                          gl3::PhysicsCategories::CATEGORY_SPEAKER |
                                              gl3::PhysicsCategories::CATEGORY_PLAYER |
                                              gl3::PhysicsCategories::CATEGORY_SWITCH;

        // 5. Create and Attach the Shape (THE CRASH POINT)
        shape = b2CreateCircleShape(
            body,
            &botShapeDef,
            &circleGeometry
        );
        // Right after creating the shape in Ship::createPhysicsBody()
        b2Filter filter = b2Shape_GetFilter(shape);

        // 6. Set Friction and Restitution (USING YOUR SETTER FUNCTIONS)
        b2Shape_SetFriction(shape, 0.0f);
        b2Shape_SetRestitution(shape, 0.1f);

        // 7. Prevent Rotation
        b2MassData massData = b2Body_GetMassData(body);
        massData.rotationalInertia = 0.0f;
        b2Body_SetMassData(body, massData);
        // If the constant is missing, we define the struct manually
        b2MotionLocks locks = {0};
        locks.angularZ = true; // This is the internal bitfield
        b2Body_SetMotionLocks(body, locks);
        // b2Body_SetGravityScale(body, 0.0f); // Disable gravity

        // After creating body
        b2Vec2 vel = b2Body_GetLinearVelocity(body);
        //std::cout << "Initial bot velocity: " << vel.x << ", " << vel.y << std::endl;
        b2Shape_EnableSensorEvents(shape, true);
        b2Shape_EnableContactEvents(shape, true);
        b2Shape_SetUserData(shape, this);

    }


    void SecBot::startContact(Entity* other) {
        std::cout << "[DEBUG] Contact with: " << (int)other->getType() << std::endl;
        if (!isTurning) isTurning = true;
        timeRemainingBeforeTurn = turnTimer;

        // STOP the bot during turn
        b2Body_SetLinearVelocity(body, {0.0f, 0.0f});

    }

    void SecBot::flipSpriteDirection() {
        patrolDirection = patrolDirection > 0 ? -1 : 1;
        std::cout << patrolDirection << std::endl;
        sprite->setScale(glm::vec3(patrolDirection * abs(sprite->getScale().x), sprite->getScale().y, sprite->getScale().z));
        isTurning = false;
        timeRemainingBeforeTurn = turnTimer;
    }

    void SecBot::castRay() {
        b2Vec2 botPosi = b2Body_GetPosition(body);

        // Setup coordinates specific to SecBot
        float eyeOffset = 0.2f;
        b2Vec2 origin = {botPosi.x, botPosi.y + eyeOffset};
        b2Vec2 end = {origin.x + (patrolDirection * 1.5f), origin.y};

        // Specify what the ray is and what it looks for
        uint64_t category = gl3::PhysicsCategories::CATEGORY_RAY_SENSOR;
        uint64_t mask = gl3::PhysicsCategories::CATEGORY_PLAYER |
                        gl3::PhysicsCategories::CATEGORY_WALL |
                        gl3::PhysicsCategories::CATEGORY_DOOR |
                        gl3::PhysicsCategories::CATEGORY_BOX;

        // Call the global helper!
        RayCastResult result = PhysicsUtils::CastRay(physicsWorld, origin, end, mask, category);

        // Handle SecBot's specific state reactions
        if (result.didHit && result.hitEntity) {
            if (result.hitEntity->getType() == EntityType::Player) {
                playerSpotted = true;
                isViewBlocked = false;
                // std::cout << "PLAYER SPOTTED" << std::endl;
            } else {
                if (result.hitEntity->getType() == EntityType::Wall) {
                    // std::cout << "WALL SPOTTED" << std::endl;
                }
                playerSpotted = false;
                isViewBlocked = true;
            }
        } else {
            playerSpotted = false;
            isViewBlocked = false;
        }
    }

    bool SecBot::checkForGroundAhead() {
        b2Vec2 botPos = b2Body_GetPosition(body);


        // 1. Set up the origin and end points exactly as you had them

        b2Vec2 origin = {botPos.x + (patrolDirection * 0.5f), botPos.y - 0.4f};

        b2Vec2 end = {botPos.x + (patrolDirection * 0.5f), botPos.y - 1.0f};


        // 2. Define the configuration masks

        uint64_t category = gl3::PhysicsCategories::CATEGORY_RAY_SENSOR;

        uint64_t mask = gl3::PhysicsCategories::CATEGORY_FLOOR; // Only look for the floor!


        // 3. Call your global utility function

        RayCastResult groundResult = PhysicsUtils::CastRay(physicsWorld, origin, end, mask, category);


        // 4. Evaluate the hit flag or the hit entity

        if (!groundResult.didHit) {
            std::cout << "[DEBUG] Gap detected! Turning." << std::endl;

            return true;
        }
        return false;
    }


    bool SecBot::canReachPlayer() {
        // If you use -1 for "no floor", check against that.
        // Otherwise, just compare the IDs directly.
        return (this->floorID == ship->getCurrentFloorId());
    }




    void SecBot::Freeze() {
        isFrozen = true;
        std::cout << isFrozen << std::endl;
        b2Body_SetLinearVelocity(body, b2Vec2_zero);

    }

    void SecBot::UnFreeze() {
        this->isFrozen = false;
        // Wake up the body and give it a tiny nudge or
        // re-trigger your patrol velocity logic
        b2Body_SetAwake(this->body, true);
    }





    // --- In SecBot.cpp ---
}