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


    Ship::Ship(Shader *shader, glm::vec3 position, glm::vec3 scale, int id, b2WorldId physicsWorld) : Entity(
        shader,
        position,
        scale,
        physicsWorld,
        EntityType::Player),
    floorID(id) {
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

        b2Vec2 shipPos = b2Body_GetPosition(body);
        // std::cout << "[SHIP] pos=(" << shipPos.x << "," << shipPos.y
        //           << ") category=" << gl3::PhysicsCategories::CATEGORY_PLAYER << std::endl;


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
        direction = (sprite->getScale().x > 0) ? 1.0f : -1.0f;
        // Flip sprite horizontally based on which movement key was  pressed
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            //move right, face right
            sprite->setScale(glm::vec3(abs(sprite->getScale().x), sprite->getScale().y, sprite->getScale().z));
            direction = 1.0f;
        } else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            //move left, face left
            sprite->setScale(glm::vec3(-abs(sprite->getScale().x), sprite->getScale().y, sprite->getScale().z));
            direction = -1.0f;
        }

        // Left Shift key for pushing
        push_PullKeyPressed = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;

        playerVelX = vel.x;


        // updateBasedOnPhysics();
        playerPos = position;
        // Prevent any rotation
        b2Body_SetAngularVelocity(body, 0);


        bool canJump = (abs(vel.y) < 0.1f); // effectively zero
        bool spacePressed = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
        bool spaceJustPressed = spacePressed && !spaceWasPressed;
        spaceWasPressed = spacePressed;
        float ladderTop = 0.0f;
        if (currentLadder != nullptr) {
            ladderTop = currentLadder->getTopY();
        }
        float feetY = position.y - playerHalfHeight;
        // --- 1. Detect if we should start or stop climbing ---
        // --- CLIMBING LOGIC ---
        if (this->canClimb && currentLadder != nullptr) {
            bool moveUp = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
            holdingDown = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;


            if (!isClimbing) {
                //can´t combine these because it´ll make the player glitch a little into the floor before going up
                if (moveUp && feetY < (ladderTop - 0.3f)) {
                    isClimbing = true;
                } else if (holdingDown && feetY >= (ladderTop - 0.5f)) {
                    isClimbing = true;
                    // Move down slightly to clear floor.... DOESNT SEEM NECESSARY
                    //b2Body_SetTransform(body, {position.x, position.y - 0.15f}, b2MakeRot(0.0f));
                }
            }
            if (spaceJustPressed) isClimbing = false;
        } else {
            isClimbing = false;
        }

        // --- APPLY CLIMBING PHYSICS ---
        if (isClimbing && currentLadder != nullptr) {
            b2Body_SetGravityScale(body, 0.0f);
            b2Shape_SetFriction(shape, 0.0f);

            if (feetY > ladderTop && vel.y > 0.1f) {
                isClimbing = false;
                b2Body_SetGravityScale(body, 1.0f);
                b2Shape_SetFriction(shape, 0.5f);
                vel.y = 0.0f;
                b2Body_SetTransform(body, {position.x, ladderTop + playerHalfHeight + 0.1f}, b2MakeRot(0.0f));
            } else {
                // MOVEMENT KEYS
                if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) vel.y = 3.0f;
                else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) vel.y = -3.0f;
                else vel.y = 0.0f;
            }
        } else {
            // --- NORMAL PHYSICS ---
            b2Body_SetGravityScale(body, 1.0f);
            b2Shape_SetFriction(shape, 0.5f);
            if (spaceJustPressed && canJump) {
                b2Body_ApplyLinearImpulseToCenter(body, {0.0f, 5.0f}, true);
                vel = b2Body_GetLinearVelocity(body); // Re-fetch after impulse
            }
        }

        if (currentSpeakerSwitch) {
            bool isFPressed = (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS);

            // Only trigger if F is pressed AND we haven't 'locked' it yet
            if (isFPressed && !F_WasPressed) {
                std::cout << "--- MANUAL TOGGLE TRIGGERED ---" << std::endl;
                currentSpeakerSwitch->flipSwitch();
                F_WasPressed = true; // Lock it
            } else if (!isFPressed) {
                F_WasPressed = false; // Unlock when player lets go
            }
        }
        if (correspVent) {
            bool isVPressed = (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS);

            if (isVPressed && !V_wasPressed) {
                if (!correspVent->getIsOpen()) {
                    correspVent->Open();
                } else if (correspVent->getIsOpen() && myStick && myStick->getIsFollowingPlayer()) {
                    Vent *connectedTwin = correspVent->getConnectedTwin();
                    if (connectedTwin != nullptr) {
                        myStick->teleport(game, connectedTwin->getPosition());

                        std::cout << "ALL CONDITIONS MET TO PUT STICK THROUGH VENT" << std::endl;
                    }
                }
                V_wasPressed = true;
            } else if (!isVPressed) {
                V_wasPressed = false;
            }
        }
        if (myStick) {
            bool isEPressed = (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS);

            if (isEPressed && !E_wasPressed) {
                // CASE 1: We are already holding the stick -> Drop it immediately!
                if (myStick->getIsFollowingPlayer()) {
                    std::cout << "MANUALLY DROPPING STICK" << std::endl;
                    myStick->dropStick(position, direction);
                }
                // CASE 2: The stick is on the ground -> Check distance mathematically
                else {
                    glm::vec3 stickPos = myStick->getPosition();
                    float currentDistance = glm::distance(position, stickPos);

                    // Define your pickup radius threshold (1.0f is usually perfect for 2D)
                    const float PICKUP_RANGE = 1.0f;

                    if (currentDistance <= PICKUP_RANGE) {
                        std::cout << "PICKING UP STICK (Distance Check Passed: " << currentDistance << ")" << std::endl;
                        myStick->pickUpStick(game, position, direction);
                    } else {
                        std::cout << "Too far away to pick up! Distance: " << currentDistance << std::endl;
                    }
                }
                E_wasPressed = true;
            } else if (!isEPressed) {
                E_wasPressed = false;
            }

            // --- Handling Aiming & Throwing ---
            bool LMBisDown = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
            bool RMBisDown = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;

            if (myStick->getIsFollowingPlayer()) {
                if (LMBisDown && !LMBwasDown) {
                    isAiming = !isAiming;
                    myStick->setAiming(isAiming);
                }
            } else {
                isAiming = false;
                myStick->setAiming(false);
            }
            LMBwasDown = LMBisDown;

            if (isAiming && myStick->getIsFollowingPlayer()) {
                myStick->simulateThrow(game, position, direction);

                if (RMBisDown && !RMBwasDown) {
                    std::cout << "THROWING STICK" << std::endl;
                    isAiming = false;
                    myStick->setAiming(false);
                    myStick->throwStick(game, position, direction);
                }
                RMBwasDown = RMBisDown;
            }

            if (myStick->getIsFollowingPlayer()) {
                myStick->updateHoldingPosition(position);
            }
        }


        // --- FINAL STEP: CRITICAL ---
        // This MUST be the last line of update()
        b2Body_SetLinearVelocity(body, vel);
    }

    void Ship::createPhysicsBody() {
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

        // 1. Define dimensions
        playerHalfWidth = scale.x / 2.0f;
        playerHalfHeight = scale.y / 2.0f;

        // 2. Create a standard box instead of a complex hull
        b2Polygon box = b2MakeBox(playerHalfWidth, playerHalfHeight);

        // 3. Create the shape
        b2ShapeDef shapeDef = b2DefaultShapeDef();
        shapeDef.userData = this; // ← add this line
        shapeDef.filter.categoryBits = gl3::PhysicsCategories::CATEGORY_PLAYER;
        std::cout << "CATEGORY BITS FOR PLAYER " << shapeDef.filter.categoryBits << std::endl;
        // Make sure this mask matches exactly what your Bot's raycast filter allows
        shapeDef.filter.maskBits = gl3::PhysicsCategories::CATEGORY_FLOOR |
                                   gl3::PhysicsCategories::CATEGORY_WALL |
                                   gl3::PhysicsCategories::CATEGORY_BOX |
                                   gl3::PhysicsCategories::CATEGORY_DOOR |
                                   gl3::PhysicsCategories::CATEGORY_SWITCH |
                                   gl3::PhysicsCategories::CATEGORY_SPEAKER_SWITCH |
                                   gl3::PhysicsCategories::CATEGORY_LADDER |
                                   gl3::PhysicsCategories::CATEGORY_VENT |
                                   gl3::PhysicsCategories::CATEGORY_STICK |
                                   gl3::PhysicsCategories::CATEGORY_RAY_SENSOR;


        shape = b2CreatePolygonShape(body, &shapeDef, &box);
        b2Filter f = b2Shape_GetFilter(shape);
        std::cout << "[SHIP SHAPE] categoryBits=" << f.categoryBits
                << " maskBits=" << f.maskBits << std::endl;

        b2Shape_SetFriction(shape, 0.5f);
        b2Shape_SetRestitution(shape, 0.0f); // optional: no bounce
        b2Shape_SetDensity(shape, 2.0f, true);
        b2Shape_EnableContactEvents(shape, true);
        b2Shape_EnableSensorEvents(shape, true);
        b2Shape_SetUserData(shape, this);

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

    void Ship::startContact(Entity *other) {
        if (other && other->getType() == EntityType::Floor) {

            Floor* f = dynamic_cast<Floor*>(other);

            // Only update if the object's top is below our center
            // or specifically at our feet
            if (f->getPosition().y < this->getPosition().y) {
                currentFloor = f;
                floorID = currentFloor->getFloorID();
            }
        }
    }


    void Ship::kill(Game *game) {
        isDead = true;
        std::cout << "Player Died" << std::endl;
        game->requestSceneSwitch(game->getIdToLoad());
    }
}
