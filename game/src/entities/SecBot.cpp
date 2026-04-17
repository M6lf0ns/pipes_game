#include "SecBot.h"
#include <iostream>
#include "../Assets.h"
#include "../Game.h"
#include "../rendering/Texture.h"
#include "box2d/box2d.h"
#include "box2d/types.h"
#include "../PhysicsCategories.h"



namespace gl3 {

    struct RayCastResult {
    b2ShapeId shapeId = {0};
    b2Vec2 hitPoint = {0.0f, 0.0f};
    b2Vec2 normal = {0.0f, 0.0f};
    float distanceAlongRay = 1.0f;  // 1.0 = no hit
    Entity* hitEntity = nullptr;

};

    //Function called for each Ray Hit
    float MyCastCallback(b2ShapeId shapeId, b2Vec2 point, b2Vec2 normal, float fraction, void* context)
    {

        RayCastResult* myContext = (RayCastResult*)context;

        if (fraction < myContext->distanceAlongRay) { //is the current hit CLOSER than the best we´ve found so far?
            //if so, overwrite data
            myContext->shapeId = shapeId;
            myContext->hitPoint = point;
            myContext->normal = normal;
            myContext->distanceAlongRay = fraction; //best/closest hit found SO FAR




            void* userData = b2Shape_GetUserData(shapeId);
            if (userData) {
                myContext->hitEntity = static_cast<Entity*>(userData);
            }
            else {
                myContext->hitEntity = nullptr;
            }
        }

        //Tells box2D to only search from start to THIS point for even closer hits
        return fraction; //current hit's distance along ray (0.0 at start → 1.0 at end)
    }

    SecBot::SecBot(Shader* shader, glm::vec3 position, glm::vec3 scale, glm::vec4 color, b2WorldId physicsWorld) : Entity (
        shader,
        position,
        scale,
        color,
        physicsWorld,
        EntityType::SecBot)

        {
        this->sprite = new Sprite(position, scale, Texture::LoadTexture("assets/textures/test_secBot.png"));
        SecBot::createPhysicsBody();

    }


    void SecBot::update(Game *game, float deltaTime) {
        if (isTurning) {

            timeRemainingBeforeTurn -= deltaTime;
           // std::cout << "time remaining before turn " << timeRemainingBeforeTurn << std::endl;
            // STOP all movement while turning
            b2Body_SetLinearVelocity(body, {0.0f, 0.0f});

            if (timeRemainingBeforeTurn <= 0.0f) {
                flipSpriteDirection();

            }
            return;
        }
        auto ship = game->getShip();
        if (ship) {
            if (playerSpotted && !(game->getShip()->isDead)) {
                detectionTimer += deltaTime;
                if (detectionTimer >= TIME_TO_KILL) {
                    game->getShip()->kill(game);
                }
            } else {
                detectionTimer = 0.0f; // Reset if they hide in time
            }
        }


        castRay();
        //NORMAL MOVEMENT
        float targetVelocity = PATROL_SPEED * patrolDirection;
        float PROPORTIONAL_GAIN = 5.0f;
        float MAX_FORCE = 1.1f; //fix it later
        b2Vec2 currentSpeed = b2Body_GetLinearVelocity(body);
        //we already have patrolSPeed
        float velocityError = targetVelocity - currentSpeed.x;


        float forceX = velocityError * PROPORTIONAL_GAIN;
        forceX = std::clamp(forceX, -MAX_FORCE, MAX_FORCE); //make sure force doesn´t exceed a certain threshold


        b2Body_ApplyForceToCenter(body, {forceX, 0.0f}, true);

        //std::cout << "Target: " << targetVelocity << " | Current: " << currentSpeed.x << " | Force: " << forceX << std::endl;


    }

    void SecBot::createPhysicsBody() {

        // 1. Calculate the Radius (0.5m)
        botRadius = 0.5f;

        // 2. Define and Create Body
        b2BodyDef botBodyDef = b2DefaultBodyDef();
        botBodyDef.type = b2_dynamicBody;
        botBodyDef.position = {position.x, position.y};
        botBodyDef.userData = this;
        body = b2CreateBody(physicsWorld, &botBodyDef);

        // 3. Define the Shape Geometry
        // *** Safer Initialization using struct assignment (C++11/14) ***
        circleGeometry = { {0.0f, 0.0f}, botRadius };
        // This explicitly initializes center to {0,0} and radius.

        // 4. Define the Shape Properties
        b2ShapeDef botShapeDef = b2DefaultShapeDef();
        botShapeDef.density = 1.0f;
        botShapeDef.userData = this; // Must be set before creation
        botShapeDef.filter.categoryBits = gl3::PhysicsCategories::CATEGORY_SEC_BOT;
        botShapeDef.filter.maskBits = gl3::PhysicsCategories::CATEGORY_FLOOR |
                                  gl3::PhysicsCategories::CATEGORY_WALL |
                                  gl3::PhysicsCategories::CATEGORY_BOX |
                                      gl3::PhysicsCategories::CATEGORY_DOOR;

        // 5. Create and Attach the Shape (THE CRASH POINT)
        shape = b2CreateCircleShape(
            body,
            &botShapeDef,
            &circleGeometry
        );

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

        // After creating body
        b2Vec2 vel = b2Body_GetLinearVelocity(body);
        //std::cout << "Initial bot velocity: " << vel.x << ", " << vel.y << std::endl;

    }


    void SecBot::startContact() {
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
        b2QueryFilter filter = b2DefaultQueryFilter(); //filters what the ray CAN hit
        //need to specify door, box and walls so bot doesnt detect player through these objects
        filter.maskBits = gl3::PhysicsCategories::CATEGORY_PLAYER |
                          gl3::PhysicsCategories::CATEGORY_WALL |
                          gl3::PhysicsCategories::CATEGORY_DOOR |
                          gl3::PhysicsCategories::CATEGORY_BOX;

        RayCastResult result;
        b2Vec2 start = {b2Body_GetPosition(body).x + botRadius * patrolDirection, b2Body_GetPosition(body).y -0.2f}; //ray needs to start at the outer edge of the circle and needs to point in patrolDirection, and should be relatively "Eye"-Level
        b2Vec2 origin = start; //make sure the Ray is at the edge of the bot AND looking in patrolDirection AND at the height of the eye
        b2Vec2 end(start + (b2Vec2){patrolDirection * 0.45f, 0.0f});
        b2Vec2 translation = b2Sub(end, origin); //creates a vector from origin to end
        b2World_CastRay(physicsWorld, origin, translation, filter, MyCastCallback, &result);


        if (result.hitEntity &&
            result.hitEntity->getType() == EntityType::Player) {
            playerSpotted = true;
        //std::cout << "PLAYER SPOTTED!!!" << std::endl;
        }
        else {
            playerSpotted = false;
        }


    }




// --- In SecBot.cpp ---



    }





