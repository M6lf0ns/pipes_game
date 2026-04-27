#include "Ship.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <iostream>
#include "../Game.h"
#include "../Assets.h"
#include "../rendering/Texture.h"
#include "glm/gtx/string_cast.hpp"
#include "../PhysicsCategories.h"
#include "Stick.h"


namespace gl3 {

    // bool gl3::Ship::push_PullKeyPressed = false;
    // bool gl3::Ship::spaceWasPressed = false;
    // float Ship::playerHalfWidth; // Or set your default value
    // float Ship::playerHalfHeight;
    // float gl3::Ship::playerVelX;
    // glm::vec3 gl3::Ship::playerPos;


    Ship::Ship(Shader* shader, glm::vec3 position, glm::vec3 scale, b2WorldId physicsWorld) : Entity(
            shader,
            position,
            scale,
            physicsWorld,
            EntityType::Player)
    {
        this->sprite = new Sprite(position, scale, Texture::LoadTexture("assets/textures/char_test.png")); // <—
        this->setZOrder(2);
        audio.init();
        audio.setGlobalVolume(0.1f);
        firingSound.load(resolveAssetPath("audio/shot.mp3").string().c_str());
        firingSound.setSingleInstance(true);
        Ship::createPhysicsBody();

    }

    void Ship::update(Game *game, float deltaTime) {
        auto window = game->getWindow();

        if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }


        vel = b2Body_GetLinearVelocity(body); //get current velocity
        // --- START: NEW MOVEMENT AND SPRINT SYSTEM ---

        // Constants (Used only in this block)
        const float SPRINT_SPEED = 2.0f;
        const float NORMAL_SPEED = 1.1f;
        const float PROPORTIONAL_GAIN = 35.0f; // Tweak this for snappiness

        bool sprintInput = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS;
        bool isMoving = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;

        float currentMaxSpeed = NORMAL_SPEED; // Default speed


        // Check SPRINT: Requires key, movement
        if (sprintInput && isMoving) {

            currentMaxSpeed = SPRINT_SPEED;
        }



        // 2. TARGET CALCULATION (Sets desiredX)
        float desiredX = 0.0f;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            desiredX = -currentMaxSpeed;
        } else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            desiredX = currentMaxSpeed;
        }

        // 3. APPLY PROPORTIONAL FORCE
        // Check if we are NOT pushing/pulling.
        if (!push_PullKeyPressed) {

            // Only apply proportional control when free moving.
            if (std::abs(desiredX) > 0.0f || std::abs(vel.x) > 0.0f) {

                float velocityError = desiredX - vel.x;
                float forceX = velocityError * PROPORTIONAL_GAIN;

                b2Body_ApplyForceToCenter(body, {forceX, 0.0f}, true);
            }
        } else {
            // PUSH/PULL IS ACTIVE: Set the player's velocity directly.
            // We bypass proportional control and force the player's velocity to match the desired speed.
            // This gives the Box::update function a reliable target velocity (playerVelX) to follow.
            b2Body_SetLinearVelocity(body, {desiredX, vel.y});
        }
        // NOTE: If you apply a force here, it will fight the box's kinematic control.
        // It's safest to let the Box logic handle the movement for both.


        // IMPORTANT: You may need to refresh 'vel' here before the sprite flip
        vel = b2Body_GetLinearVelocity(body);

        // Initialize it so it's never "garbage"
        float direction = (sprite->getScale().x > 0) ? 1.0f : -1.0f;
        // Flip sprite horizontally based on which movement key was  pressed
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) { //move right, face right
            sprite->setScale(glm::vec3(abs(sprite->getScale().x), sprite->getScale().y, sprite->getScale().z));
            direction = 1.0f;
        } else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) { //move left, face left
            sprite->setScale(glm::vec3(-abs(sprite->getScale().x), sprite->getScale().y, sprite->getScale().z));
            direction = -1.0f;
        }

        bool canJump = (abs(vel.y) < 0.1f); // effectively zero
        bool spacePressed = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
        bool spaceJustPressed = spacePressed && !spaceWasPressed;
        spaceWasPressed = spacePressed;

        if (spaceJustPressed && canJump) {
            b2Body_ApplyLinearImpulseToCenter(body, {0.0f, 3.0f}, true);
        }
        // Left Shift key for pushing
        push_PullKeyPressed = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;

        playerVelX = vel.x;


        bool LMBisDown = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS; //toggle aim
        bool RMBisDown = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS; //throw
        //Toggle throwing simulation
        if (LMBisDown && !LMBwasDown) {
            isAiming = !isAiming;
            myStick->setAiming(isAiming);
        }
        LMBwasDown = LMBisDown;

        if (isAiming) {
            myStick->simulateThrow(game, position, direction);

            if (RMBisDown && !RMBwasDown) {
                std::cout << "THROWING STICK " << std::endl;
                myStick->throwStick(game, position, direction);
                isAiming = false;
                myStick->setAiming(isAiming);
            }
            RMBwasDown = RMBisDown;

        }


            // updateBasedOnPhysics();

            playerPos = position;


            // Prevent any rotation
            b2Body_SetAngularVelocity(body, 0);

        if (this->canClimb) {
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
                b2Body_SetGravityScale(body, 0.0f);

                // Manual movement logic
                if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) vel.y = 3.0f;
                else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) vel.y = -3.0f;
                else vel.y = 0.0f; // Stay still on the ladder!
            }
        } else {
            // RESET GRAVITY HERE
            // Player is no longer on a ladder, ensure physics are back to normal
            if (!this->canClimb) {
                b2Body_SetGravityScale(body, 1.0f);

            }
        }

        b2Body_SetLinearVelocity(body, vel);
        }




    void Ship::createPhysicsBody()
    {

        std::cout << "Box: physicsWorld.index1 = " << physicsWorld.index1 << std::endl;

        if (physicsWorld.index1 < 1 || physicsWorld.index1 > 32) {
            std::cout << "ERROR: Box has invalid physicsWorld!" << std::endl;
            throw std::runtime_error("Invalid physics world in Box");
        }
        b2BodyDef bodyDef = b2DefaultBodyDef();
        bodyDef.enableSleep = false; // Prevent body from sleeping
        bodyDef.type = b2_dynamicBody;
        bodyDef.position = {position.x, position.y};
        bodyDef.userData = this;
        body = b2CreateBody(physicsWorld, &bodyDef);
        // Use the actual scale passed from Game.cpp
        playerHalfWidth = scale.x / 3.0f;
        playerHalfHeight = scale.y / 2.0f;

        // 1. Define the vertices as usual
        b2Vec2 vertices[3];
        vertices[0] = {-playerHalfWidth,  playerHalfHeight}; // Top Left
        vertices[1] = { playerHalfWidth,  playerHalfHeight}; // Top Right
        vertices[2] = { 0.0f,            -playerHalfHeight}; // Bottom Tip

        // 2. Compute the Hull (This is what was missing)
        b2Hull hull = b2ComputeHull(vertices, 3);

        // 3. Create the Polygon using the Hull and a "radius" (usually 0.0f)
        b2Polygon triangle = b2MakePolygon(&hull, 0.0f);

        // 4. Everything else stays exactly as you had it
        b2ShapeDef shapeDef = b2DefaultShapeDef();
        shapeDef.filter.categoryBits = gl3::PhysicsCategories::CATEGORY_PLAYER;
        shapeDef.filter.maskBits = gl3::PhysicsCategories::CATEGORY_FLOOR |
                                   gl3::PhysicsCategories::CATEGORY_WALL  |
                                   gl3::PhysicsCategories::CATEGORY_BOX   |
                                   gl3::PhysicsCategories::CATEGORY_DOOR  |
                                   gl3::PhysicsCategories::CATEGORY_SWITCH;

        shape = b2CreatePolygonShape(body, &shapeDef, &triangle);

        // ... keep your EnableContactEvents, SetUserData, etc. below this ...
        b2Shape_EnableContactEvents(shape, true);
        b2Shape_EnableSensorEvents(shape, true);
        b2Shape_SetUserData(shape, this);
        b2Shape_SetFriction(shape, 0.5f);
        b2Shape_SetRestitution(shape, 0.0f); // optional: no bounce
        b2Shape_SetDensity(shape, 2.0f, true);

        // Debug
        b2AABB aabb = b2Shape_GetAABB(shape);
        std::cout << "Fixed Ship AABB: " << (aabb.upperBound.x - aabb.lowerBound.x)
                  << " x " << (aabb.upperBound.y - aabb.lowerBound.y) << std::endl;


        std::cout << "AABB Dimensions: " << (aabb.upperBound.x - aabb.lowerBound.x)
                  << " x " << (aabb.upperBound.y - aabb.lowerBound.y) << std::endl;
        std::cout << "Expected dimensions: 0.25 x 0.25" << std::endl;

        // Also check the body transform
        auto transform = b2Body_GetTransform(body);
        std::cout << "Body transform - position: (" << transform.p.x << ", " << transform.p.y
                  << ") rotation: " << transform.q.s << std::endl;

        b2MassData massData;
        massData.mass = 1.0f;
        massData.center = b2Vec2{0.0f, 0.0f};
        massData.rotationalInertia = 0.0f; // must be explicitly set
        b2Body_SetMassData(body, massData);

        b2Body_SetLinearDamping(body, 0.5f); // tweak this value



    }



    void Ship::setStick(Stick *stick) {
        this->myStick = stick;
    }

    void Ship::pickUpStick(Stick *stick) {

    }

    void Ship::climb() {


    }






    void Ship::startContact(Entity* other) {
        std::cout << "Ship start contact" << std::endl;
    }


    void Ship::kill(Game* game) {
        isDead = true;
        std::cout << "Player Died" << std::endl;
        b2Body_Disable(body);
        game->isGameOver = true;

    }

}
