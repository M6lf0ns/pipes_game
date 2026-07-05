#include "Ship.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <iostream>
#include "../Game.h"
#include "../Assets.h"
#include "../rendering/Texture.h"
#include "glm/gtx/string_cast.hpp"
#include "../PhysicsCategories.h"
// #include "Stick.h"
#include "Box.h"



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
        this->sprite = new Sprite(position, scale, Texture::LoadTexture("assets/textures/Accelerating_1.png")); // <—
        this->setZOrder(2);

        idleTexture = Texture::LoadTexture("assets/textures/Accelerating_1.png");

        pushingFrames.push_back(Texture::LoadTexture("assets/textures/Pushing_1.png"));
        pushingFrames.push_back(Texture::LoadTexture("assets/textures/Pushing_2.png"));
        pushingFrames.push_back(Texture::LoadTexture("assets/textures/Pushing_3.png"));
        pushingFrames.push_back(Texture::LoadTexture("assets/textures/Pushing_4.png"));
        pushingFrames.push_back(Texture::LoadTexture("assets/textures/Pushing_5.png"));
        pushingFrames.push_back(Texture::LoadTexture("assets/textures/Pushing_6.png"));

        jumpingFrames.push_back(Texture::LoadTexture("assets/textures/Jump_1.png"));
        jumpingFrames.push_back(Texture::LoadTexture("assets/textures/Jump_2.png"));
        jumpingFrames.push_back(Texture::LoadTexture("assets/textures/Jump_3.png"));
        jumpingFrames.push_back(Texture::LoadTexture("assets/textures/Jump_4.png"));
        // jumpingFrames.push_back(Texture::LoadTexture("assets/textures/Jump_5.png"));
        // jumpingFrames.push_back(Texture::LoadTexture("assets/textures/Jump_6.png"));
        // jumpingFrames.push_back(Texture::LoadTexture("assets/textures/Jump_7.png"));
        jumpingFrames.push_back(Texture::LoadTexture("assets/textures/Accelerating_1.png"));

        climbLadderFrames.push_back(Texture::LoadTexture("assets/textures/ladder_climb_1.png"));
        climbLadderFrames.push_back(Texture::LoadTexture("assets/textures/ladder_climb_2.png"));
        climbLadderFrames.push_back(Texture::LoadTexture("assets/textures/ladder_climb_3.png"));
        climbLadderFrames.push_back(Texture::LoadTexture("assets/textures/ladder_climb_4.png"));
        climbLadderFrames.push_back(Texture::LoadTexture("assets/textures/ladder_climb_5.png"));

        climbBoxFrames.push_back(Texture::LoadTexture("assets/textures/Jump_On_Box_1.png"));
        climbBoxFrames.push_back(Texture::LoadTexture("assets/textures/Jump_On_Box_2.png"));
        climbBoxFrames.push_back(Texture::LoadTexture("assets/textures/Jump_On_Box_3.png"));
        climbBoxFrames.push_back(Texture::LoadTexture("assets/textures/Jump_On_Box_4.png"));
        climbBoxFrames.push_back(Texture::LoadTexture("assets/textures/Jump_On_Box_5.png"));
        climbBoxFrames.push_back(Texture::LoadTexture("assets/textures/Jump_On_Box_6.png"));
        climbBoxFrames.push_back(Texture::LoadTexture("assets/textures/Jump_On_Box_7.png"));

        acceleratingFrames.push_back(Texture::LoadTexture("assets/textures/Accelerating_1.png"));
        acceleratingFrames.push_back(Texture::LoadTexture("assets/textures/Accelerating_2.png"));
        acceleratingFrames.push_back(Texture::LoadTexture("assets/textures/Accelerating_3.png"));
        acceleratingFrames.push_back(Texture::LoadTexture("assets/textures/Accelerating_4.png"));

        deceleratingFrames.push_back(Texture::LoadTexture("assets/textures/Braking_1.png"));
        deceleratingFrames.push_back(Texture::LoadTexture("assets/textures/Braking_2.png"));
        deceleratingFrames.push_back(Texture::LoadTexture("assets/textures/Braking_3.png"));
        deceleratingFrames.push_back(Texture::LoadTexture("assets/textures/Braking_4.png"));

        continuousSpeedFrames.push_back(Texture::LoadTexture("assets/textures/Continuous_Speed_1.png"));
        continuousSpeedFrames.push_back(Texture::LoadTexture("assets/textures/Continuous_Speed_2.png"));
        continuousSpeedFrames.push_back(Texture::LoadTexture("assets/textures/Continuous_Speed_3.png"));
        continuousSpeedFrames.push_back(Texture::LoadTexture("assets/textures/Continuous_Speed_4.png"));
        continuousSpeedFrames.push_back(Texture::LoadTexture("assets/textures/Continuous_Speed_5.png"));
        continuousSpeedFrames.push_back(Texture::LoadTexture("assets/textures/Continuous_Speed_6.png"));
        continuousSpeedFrames.push_back(Texture::LoadTexture("assets/textures/Continuous_Speed_7.png"));

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

        bool canJump = (abs(vel.y) < 0.1f); // effectively zero
        // --- START: NEW MOVEMENT AND SPRINT SYSTEM ---

        animTimer += deltaTime;
        if (animTimer >= 0.05f) {
            animTimer = 0.0f;
            if (currentState == PlayerState::pushingBox) {
                animFrame = (animFrame + 1) % pushingFrames.size();
                this->sprite->setTexture(pushingFrames[animFrame]);
            }
            else if (currentState == PlayerState::accelerating) {
                animFrame = (animFrame + 1) % acceleratingFrames.size();
                this->sprite->setTexture(acceleratingFrames[animFrame]);
            }
            else if (currentState == PlayerState::decelerating) {
                animFrame = (animFrame + 1) % deceleratingFrames.size();
                this->sprite->setTexture(deceleratingFrames[animFrame]);
            }
            else if (currentState == PlayerState::continuousSpeed) {
                animFrame = (animFrame + 1) % continuousSpeedFrames.size();
                this->sprite->setTexture(continuousSpeedFrames[animFrame]);
            }
            else if (currentState == PlayerState::climbingLadder) {
                animFrame = (animFrame + 1) % climbLadderFrames.size();
                this->sprite->setTexture(climbLadderFrames[animFrame]);
            }
            // else if (currentState == PlayerState::jumping) {
            //     animFrame++;
            //     if (animFrame == 2) {
            //         b2Body_ApplyLinearImpulseToCenter(body, {0.0f, 5.0f}, true);
            //     }
            //
            //     if (animFrame >= jumpingFrames.size()) {
            //         animFrame = jumpingFrames.size() - 1;
            //     }
            //     this->sprite->setTexture(jumpingFrames[animFrame]);
            //
            //     // NEW: Use the 'canJump' variable to detect landing immediately
            //     // If we are grounded and have been jumping for a few frames
            //     if (canJump && animFrame >= 2) {
            //         // currentState = PlayerState::idle;
            //         // animFrame = 0;
            //     }
            // }
            else if (currentState == PlayerState::climbingBox) {
                animFrame++;

                if (animFrame == 4) {
                    // Frames 0-3 played, now teleport to top
                    float targetY = boxHeight + playerHalfHeight;
                    b2Body_SetTransform(body, {boxPositionX, targetY}, b2MakeRot(0.0f));
                    b2Body_SetLinearVelocity(body, {0.0f, 0.0f});
                    b2Body_SetGravityScale(body, 0.0f);

                    b2Filter filter = b2Shape_GetFilter(shape);
                    filter.maskBits &= ~gl3::PhysicsCategories::CATEGORY_BOX;
                    b2Shape_SetFilter(shape, filter);
                }

                if (animFrame >= climbBoxFrames.size()) {
                    // 1. Animation finished
                    currentState = PlayerState::idle;
                    animFrame = 0;

                    // 2. RESTORE GRAVITY
                    b2Body_SetGravityScale(body, 1.0f);

                    // 3. RESTORE COLLISIONS
                    b2Filter filter = b2Shape_GetFilter(shape);
                    filter.maskBits |= gl3::PhysicsCategories::CATEGORY_BOX;
                    b2Shape_SetFilter(shape, filter);

                    // 4. GIVE A TINY NUDGE (Crucial for Jump-Detection)
                    // Sometimes setting transform makes the physics engine think we are "overlapping"
                    // rather than "contacting". This velocity reset helps the engine re-calculate contacts.
                    b2Body_SetLinearVelocity(body, {0.0f, 0.0f});

                    this->sprite->setTexture(idleTexture);
                } else {
                    this->sprite->setTexture(climbBoxFrames[animFrame]);
                }
            }
            else if (currentState == PlayerState::idle) {
                this->sprite->setTexture(idleTexture);
            }

        }
        if (currentState == PlayerState::climbingBox && animFrame < 4) {
            b2Body_SetLinearVelocity(body, {0.0f, 0.0f});
            b2Body_SetGravityScale(body, 0.0f);
        }
        if (currentState == PlayerState::climbingBox && animFrame >= 4) {
            b2Body_SetTransform(body, {boxPositionX, boxHeight + playerHalfHeight}, b2MakeRot(0.0f));
            b2Body_SetLinearVelocity(body, {0.0f, 0.0f});
            b2Body_SetGravityScale(body, 0.0f);
        }

        // 1. Determine State based on Physics Velocity
        // 1. Determine State based on Physics Velocity

        // std::cout << "currentState=" << (int)currentState << " ismoving=" << ismoving << " vel.x=" << vel.x << std::endl;
        // 1. Determine State based on Physics Velocity
        if (!isClimbing && !holdingDown && !moveUp && currentState != PlayerState::climbingBox) {

            // Check Pushing FIRST
            if (push_PullKeyPressed && isFacingBox && std::abs(vel.x) > 0.05f) {
                currentState = PlayerState::pushingBox;
            }
            // Simply determine state based on movement and speed.
            // If we are in the air, the state will stay whatever it was last
            // (e.g., accelerating), which is fine since we aren't animating the jump.
            if (ismoving) {
                if (std::abs(vel.x) > 0.05f) {
                    if (std::abs(vel.x) < 0.8f) {
                        currentState = PlayerState::accelerating;
                    } else {
                        currentState = PlayerState::continuousSpeed;
                    }
                }
            } else {
                currentState = PlayerState::idle;
            }
        }


        // Constants (Used only in this block)
        const float SPRINT_SPEED = 2.0f;
        const float NORMAL_SPEED = 1.3f;
        const float PROPORTIONAL_GAIN = 30.0f; // Tweak this for snappiness

        bool sprintInput = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS;
        bool isMoving = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;
        ismoving = isMoving;

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
            if (isFacingBox) {
                // Allow the PUSH/PULL logic
                currentState = PlayerState::pushingBox;
            } else {
                // Player is trying to pull a box they are facing away from
                // Do nothing, or maybe force the player to turn around?
                currentState = PlayerState::idle;
            }
        }
        // NOTE: If you apply a force here, it will fight the box's kinematic control.
        // It's safest to let the Box logic handle the movement for both.


        // IMPORTANT: You may need to refresh 'vel' here before the sprite flip
        vel = b2Body_GetLinearVelocity(body);



        // Initialize it so it's never "garbage"
        direction = (sprite->getScale().x > 0) ? 1.0f : -1.0f;

        // 1. Determine where the box is relative to us
        bool boxIsToLeft = (playertobox < 0.0f);
        bool boxIsToRight = (playertobox > 0.0f);
        isFacingBox = boxIsToLeft || boxIsToRight;
        // Flip sprite horizontally based on which movement key was  pressed
        if (currentState != PlayerState::pushingBox) {
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS ) {
                //move right, face right
                sprite->setScale(glm::vec3(abs(sprite->getScale().x), sprite->getScale().y, sprite->getScale().z));
                direction = 1.0f;
            } else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
                //move left, face left
                sprite->setScale(glm::vec3(-abs(sprite->getScale().x), sprite->getScale().y, sprite->getScale().z));
                direction = -1.0f;
            }
            // 3. The logic:
            // If direction is 1 (Right) and playertobox is positive (Box is to the right), result is > 0 (TRUE)
            // If direction is -1 (Left) and playertobox is negative (Box is to the left), result is > 0 (TRUE)

        }


        // Left Shift key for pushing
        push_PullKeyPressed = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;



        playerVelX = vel.x;


        // updateBasedOnPhysics();
        playerPos = position;
        // Prevent any rotation
        b2Body_SetAngularVelocity(body, 0);


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
            moveUp = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
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
            // 1. Calculate the bottom Y-threshold (where the ladder ends)
            float floorTop = currentFloor->getPosition().y + currentFloor->getScale().y / 2.0f;
            bool isAtFloor = std::abs(feetY - floorTop) < 0.2f; // Close enough
            float ladderBottom = currentLadder->getPosition().y - (currentLadder->getScale().y / 2.0f);

            std::cout << "FeetY: " << feetY << " | FloorTop: " << floorTop << " | isClimbing: " << isClimbing << std::endl;

            std::cout << "LADDER BOTTOM: " << ladderBottom << std::endl;
            if (feetY > ladderTop && vel.y > 0.1f) {
                isClimbing = false;
                // 1. Kill all momentum FIRST
                b2Body_SetLinearVelocity(body, {0.0f, 0.0f});
                b2Body_ApplyLinearImpulseToCenter(body, {0.0f, 0.0f}, true);

                // 2. Snap
                b2Body_SetTransform(body, {position.x, ladderTop + playerHalfHeight + 0.1f}, b2MakeRot(0.0f));

                // 3. Restore Physics
                b2Body_SetGravityScale(body, 1.0f);
                b2Shape_SetFriction(shape, 0.5f);
            }
            // Add this right before your if-else block

            // BOTTOM EXIT (Identical structure to your Top Exit)
            else if (isClimbing && feetY <= (ladderBottom + 0.1f) && vel.y < -0.09f) {
                isClimbing = false;
                b2Body_SetGravityScale(body, 1.0f);
                b2Shape_SetFriction(shape, 0.5f);
                vel.y = 0.0f;
                // Snap to exactly the bottom of the ladder
                // b2Body_SetTransform(body, {position.x, currentFloor->GetFloorY() + playerHalfHeight}, b2MakeRot(0.0f));


                // Force the State Reset
                // currentState = PlayerState::idle;
                // animTimer = 0.03f;
            }
            else {
                // MOVEMENT KEYS
                // MOVEMENT KEYS
                currentState = PlayerState::climbingLadder;

                // Change vel.y to b2Body_SetLinearVelocity(body, {vel.x, targetY});
                float targetY = 0.0f;
                if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) targetY = 3.0f;
                else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) targetY = -3.0f;

                b2Body_SetLinearVelocity(body, {0.0f, targetY}); // Apply directly to body



            }

        }  else if (currentState != PlayerState::climbingBox ) {
            // --- NORMAL PHYSICS ---
            b2Body_SetGravityScale(body, 1.0f);
            b2Shape_SetFriction(shape, 0.5f);

            if (spaceJustPressed) {
                // Priority 1: Climbing Box
                // Reset boxPositionX if we are too far away to possibly interact


           //
           //      std::cout << "[DEBUG] abs(playertobox): " << abs(playertobox)
           // << " | isFacingBox: " << isFacingBox
           // << " | canJump: " << canJump
           // << " | currentState != Jumping: " << (curreaaantState != PlayerState::jumping)
           // << std::endl;


                bool isStableEnoughToClimb = (abs(vel.y) < 0.5f);
                if (isBoxDataValid) {
                    if (abs(playertobox) <= 0.3f) {
                        if (boxIsToLeft || boxIsToRight) {
                            if (isStableEnoughToClimb) {
                                if (position.y < playerHalfHeight + boxHeight) {
                                    std::cout << "--- ALL GATES PASSED ---" << std::endl;
                                    currentState = PlayerState::climbingBox;
                                    animFrame = 0;
                                    animTimer = 0.0f;
                                } else {
                                    std::cout << "FAILED: Height check" << std::endl;
                                }
                            } else {
                                std::cout << "FAILED: Stability check" << std::endl;
                            }
                        } else {
                            std::cout << "FAILED: Direction check" << std::endl;
                        }
                    } else {
                        std::cout << "FAILED: Distance check. playertobox: " << playertobox << std::endl;
                    }
                } else {
                    std::cout << "FAILED: Box data invalid" << std::endl;
                }
                // 1. Determine where the box is relative to us
                bool boxIsToLeft = (playertobox < 0.0f);
                bool boxIsToRight = (playertobox > 0.0f);
                std::cout  << "IS BOX TO LEFT " << boxIsToLeft << std::endl;
                std::cout << "IS BOX TO RIGHT " << boxIsToRight << std::endl;
                std::cout << "IS BOX DATA VALID "  << isBoxDataValid << std::endl;
                bool condition = position.y < playerHalfHeight + boxHeight;
                std::cout << "is condition true " << condition << std::endl;
                std::cout << "is it stable enough to climg " << isStableEnoughToClimb << std::endl;
                if (isBoxDataValid && abs(playertobox) <= 0.5f && (boxIsToLeft || boxIsToRight) && isStableEnoughToClimb && position.y < playerHalfHeight + boxHeight ) {
                    currentState = PlayerState::climbingBox;
                    animFrame = 0;
                    animTimer = 0.0f;


                    // b2Body_SetLinearVelocity(body, {0.0f, 0.0f});
                    // b2Body_SetGravityScale(body, 0.0f);
                    //
                    // b2Filter filter = b2Shape_GetFilter(shape);
                    // filter.maskBits &= ~gl3::PhysicsCategories::CATEGORY_BOX;
                    // b2Shape_SetFilter(shape, filter);
                }
                // Priority 2: Jumping (Only if NOT already climbing)
                else if (currentState != PlayerState::climbingBox && canJump) {
                    b2Body_ApplyLinearImpulseToCenter(body, {0.0f, 5.0f}, true); // Apply NOW
                    std::cout << "Jump triggered without state change" << std::endl;
                }
                // Priority 3: Ladder Exit (Already handled by your isClimbing logic)
                else if (isClimbing) {
                    isClimbing = false;
                }
            }


        }






        if (myStick != nullptr) {
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
            // <--- ADD THIS CHECK
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


        // 1. INPUT DEFINITIONS
        bool isFPressed = (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS);
        bool triggerF = (isFPressed && !F_wasPressed);

        // --- F KEY: WORLD INTERACTION (Vents, Switches) ---
        if (triggerF) {
            // Priority 1: Open/Close Vent
            if (correspVent != nullptr) {
                std::cout << "TOGGLING VENT" << std::endl;
                correspVent->Open(); // Or your toggle function
            }
            // Priority 2: Pipe Switch
            else if (currentPipeSwitch != nullptr) {
                correspMirrorCube = currentPipeSwitch->getTargetCube();
                float nextAngle = std::fmod(correspMirrorCube->getPipeRotation() + 45.0f, 360.0f);
                correspMirrorCube->rotatePipe(nextAngle);
            }
            // Priority 3: Speaker Switch
            else if (currentSpeakerSwitch != nullptr) {
                currentSpeakerSwitch->flipSwitch();
            }

            F_wasPressed = true;
        } else if (!isFPressed) {
            F_wasPressed = false;
        }






        // // bool isRPressed = (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS);
        //
        // if (currentPipeSwitch != nullptr) {
        //     correspMirrorCube = currentPipeSwitch->getTargetCube();
        //     // std::cout << "DEBUG: Attempting to rotate cube at address: " << currentPipeSwitch << std::endl;
        //     // Add the safety check here
        //     if (correspMirrorCube != nullptr && isEPressed && !this->E_wasPressed) {
        //         float currentAngle = correspMirrorCube->getPipeRotation();
        //         float nextAngle = std::fmod(currentAngle + 45.0f, 360.0f);
        //
        //         // ONE call, let the MirrorCube handle the sprite/Box2D transform
        //         correspMirrorCube->rotatePipe(nextAngle);
        //
        //         std::cout << "[ROTATION] New Angle: " << nextAngle << std::endl;
        //     }
        //
        //     // Track the key state across frames globally at the very end of update
        //     this->E_wasPressed = isEPressed;
        // }
        // else {
        //     std::cout << "PIPESWITCH NOT ASSIGNED!!" << std::endl;
        // }
        // --- FINAL STEP: CRITICAL ---
        // // This MUST be the last line of update()
        // if (currentState != PlayerState::climbingBox && !isClimbing) {
        //     b2Vec2 currentVel = b2Body_GetLinearVelocity(body);
        //
        //     // Only force the X velocity for movement,
        //     // leave Y velocity alone so gravity and jumps can work
        //     b2Body_SetLinearVelocity(body, {vel.x, currentVel.y});
        // }
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
        playerHalfWidth = scale.x / 2.5f;
        playerHalfHeight = scale.y / 2.1f;

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
                                   gl3::PhysicsCategories::CATEGORY_RAY_SENSOR |
                                       gl3::PhysicsCategories::CATEGORY_MIRROR_PIPE |
                                           gl3::PhysicsCategories::CATEGORY_GOAL_SENSOR;


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

    void Ship::onSensorEnter(Entity* visitor) {
        if (visitor) {
            if (visitor->getType() == EntityType::Floor) {

                Floor* f = dynamic_cast<Floor*>(visitor);

                // Only update if the object's top is below our center
                // or specifically at our feet
                if (f->getPosition().y < this->getPosition().y) {
                    currentFloor = f;
                    floorID = currentFloor->getFloorID();
                }
            } else if (visitor && visitor->getType() == EntityType::PipeSwitch) {
                currentPipeSwitch = dynamic_cast<PipeSwitch*>(visitor);

            }
        }


    }

    void Ship::onSensorExit(Entity* visitor) {
       if (visitor == currentPipeSwitch)
       {
           setCurrentPipeSwitch(nullptr);


       }
    }

    void Ship::startContact(Entity *other) {
        if (other) {
            if (other->getType() == EntityType::Floor) {

                Floor* f = dynamic_cast<Floor*>(other);

                // Only update if the object's top is below our center
                // or specifically at our feet
                if (f->getPosition().y < this->getPosition().y) {
                    currentFloor = f;
                    floorID = currentFloor->getFloorID();
                }
            }

        }
    }

    void Ship::endContact(Entity *other) {

    }


    void Ship::kill(Game *game) {
        isDead = true;
        std::cout << "Player Died" << std::endl;
        game->requestSceneSwitch(game->getIdToLoad());
    }
}