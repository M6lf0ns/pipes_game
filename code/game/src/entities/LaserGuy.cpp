#include "LaserGuy.h"
#include "../physics/PhysicsUtils.h"
#include "../PhysicsCategories.h"
#include "../rendering/Texture.h"
#include "box2d/box2d.h"
#include "box2d/src/body.h"
#include <iostream>
#include "MirrorCube.h"
#include "Mirror.h"
#include <thread>
#include <chrono>


namespace gl3 {
    LaserGuy::LaserGuy(Shader *shader, glm::vec3 position, glm::vec3 scale, b2WorldId physicsWorld,
                       float initAngle) : Entity(shader,
                                                 position,
                                                 scale,
                                                 physicsWorld,
                                                 EntityType::LaserGuy,
                                                 initAngle) {
        this->sprite = new Sprite(position, glm::vec3(scale.x, scale.y, scale.z),
                                  Texture::LoadTexture("assets/textures/laser_eyes.png"));
        this->setZOrder(2);


        // 1. Allocate the 3 separate laser piece sprites
        this->laserHeadSprite = new Sprite(position, scale, 0);
        this->laserTailSprite = new Sprite(position, scale, 0);
        this->laserBodySprite = new Sprite(position, scale, 0);
        this->laserCornerSprite = new Sprite(position, scale, 0);
        this->idleTexture = Texture::LoadTexture("assets/textures/laserguy_idle.png");


        laserGuyShootingFrames.push_back(Texture::LoadTexture("assets/textures/laserguy-shoot-1.png"));
        laserGuyShootingFrames.push_back(Texture::LoadTexture("assets/textures/laserguy-shoot-2.png"));
        laserGuyShootingFrames.push_back(Texture::LoadTexture("assets/textures/laserguy-shoot-3.png"));
        laserGuyShootingFrames.push_back(Texture::LoadTexture("assets/textures/laserguy-shoot-4.png"));
        laserGuyShootingFrames.push_back(Texture::LoadTexture("assets/textures/laserguy-shoot-5.png"));
        // 2. Load Head Animation Frames
        headFrames.push_back(Texture::LoadTexture("assets/textures/laser_head_1.png"));
        headFrames.push_back(Texture::LoadTexture("assets/textures/laser_head_2.png"));
        headFrames.push_back(Texture::LoadTexture("assets/textures/laser_head_3.png"));
        headFrames.push_back(Texture::LoadTexture("assets/textures/laser_head_4.png"));
        headFrames.push_back(Texture::LoadTexture("assets/textures/laser_head_5.png"));
        headFrames.push_back(Texture::LoadTexture("assets/textures/laser_head_6.png"));

        // 4. Load Tail Animation Frames
        tailFrames.push_back(Texture::LoadTexture("assets/textures/laser_end_1.png"));
        tailFrames.push_back(Texture::LoadTexture("assets/textures/laser_end_2.png"));
        tailFrames.push_back(Texture::LoadTexture("assets/textures/laser_end_3.png"));
        tailFrames.push_back(Texture::LoadTexture("assets/textures/laser_end_4.png"));
        tailFrames.push_back(Texture::LoadTexture("assets/textures/laser_end_5.png"));
        tailFrames.push_back(Texture::LoadTexture("assets/textures/laser_end_6.png"));

        // 3. Load Body Animation Frames
        bodyFrames.push_back(Texture::LoadTexture("assets/textures/laser_body_1.png"));
        bodyFrames.push_back(Texture::LoadTexture("assets/textures/laser_body_2.png"));
        bodyFrames.push_back(Texture::LoadTexture("assets/textures/laser_body_3.png"));
        bodyFrames.push_back(Texture::LoadTexture("assets/textures/laser_body_4.png"));
        bodyFrames.push_back(Texture::LoadTexture("assets/textures/laser_body_5.png"));
        bodyFrames.push_back(Texture::LoadTexture("assets/textures/laser_body_6.png"));

        edgeFrames.push_back(Texture::LoadTexture("assets/textures/corner_1.png"));
        edgeFrames.push_back(Texture::LoadTexture("assets/textures/corner_2.png"));
        edgeFrames.push_back(Texture::LoadTexture("assets/textures/corner_3.png"));
        edgeFrames.push_back(Texture::LoadTexture("assets/textures/corner_4.png"));
        edgeFrames.push_back(Texture::LoadTexture("assets/textures/corner_5.png"));
        edgeFrames.push_back(Texture::LoadTexture("assets/textures/corner_6.png"));

        LaserGuy::createPhysicsBody();

        // b2Body_SetTransform(body,{position.x, position.y},b2MakeRot(glm::radians(-180.0f)));
    }

    void LaserGuy::draw(Game *game) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));


        Entity::draw(game);


        if (currentState != LaserState::firing || !isRaycastReady) {
            sprite->setTexture(idleTexture);
            return;
        }
        sprite->setTexture(laserGuyShootingFrames[guyAnimFrame]);


        // Snapshot the data to prevent race conditions during the draw call


        std::vector<glm::vec2> pointsSnapshot = laserPoints;


        std::vector<int> mirrorSnapshot = segmentEndsOnMirror;
        std::vector<bool> cornerDrawn(pointsSnapshot.size() + 5, false);


        // Add this check after your snapshot


        if (!laserHeadSprite || !laserTailSprite || !laserBodySprite || !laserCornerSprite) {
            std::cout << "[FATAL] A sprite pointer is NULL!" << std::endl;


            return;
        }


        if (pointsSnapshot.size() < 2) return;


        // Add this safety check


        if (animFrame >= headFrames.size() || animFrame >= tailFrames.size() ||


            animFrame >= bodyFrames.size() || animFrame >= edgeFrames.size()) {
            return; // Stop the draw call, the animation data is invalid right now
        }


        this->laserHeadSprite->setTexture(headFrames[animFrame]);


        this->laserTailSprite->setTexture(tailFrames[animFrame]);


        this->laserBodySprite->setTexture(bodyFrames[animFrame]);


        this->laserCornerSprite->setTexture(edgeFrames[animFrame]);


        float laserThickness = 0.2f;


        float headWidth = 0.4f;


        float tailWidth = 0.4f;


        debugBodyDrawCount = 0;


        // To this:

        if (pointsSnapshot.size() != mirrorSnapshot.size() + 1) {
            std::cout << "[CRITICAL ERROR] Mismatch: Points=" << pointsSnapshot.size()

                    << " MirrorFlags=" << mirrorSnapshot.size() << std::endl;
        }


        // --- PASS 1: DRAW ALL BODIES ---

        size_t numSegments = mirrorSnapshot.size();


        for (size_t i = 0; i < numSegments; ++i) {
            if (i >= pointsSnapshot.size()) {
                std::cout << "[CRITICAL ERROR] Loop OOB detected at i=" << i << std::endl;


                break;
            }

            glm::vec2 start = pointsSnapshot[i];


            glm::vec2 end = pointsSnapshot[i + 1];


            float len = glm::length(end - start);


            bool hitsMirror = mirrorSnapshot[i];


            float startCrop = (i > 0) ? 0.05f : (headWidth * 0.4f);


            float endCrop = (hitsMirror) ? 0.05f : 0.02f;


            float bodyWidth = (std::max)(0.0f, len - startCrop - endCrop);

            for (size_t j = 0; j < mirrorSnapshot.size(); j++) {
                // std::cout << "mirrorSnapshot[" << j << "] = " << mirrorSnapshot[j] << std::endl;
            }


            // 1. Calculate current segment direction

            glm::vec2 currentDir = glm::normalize(pointsSnapshot[i + 1] - pointsSnapshot[i]);

            // 2. The "Source-Return" check
            // If we are at index 'i', we check if we have already passed
            // a point close to our current position.
            bool isReturning = false;
            for (size_t j = 0; j < i; ++j) {
                if (glm::distance(pointsSnapshot[i], pointsSnapshot[j]) < 0.1f) {
                    isReturning = true;

                    break;
                }
            }


            if (bodyWidth > 0.0f && !isReturning) {
                glm::vec2 dir = glm::normalize(end - start);


                float angle = std::atan2(dir.y, dir.x) * (180.0f / 3.14159f);


                glm::vec2 bodyCenter = start + (dir * (startCrop + (bodyWidth / 2.0f)));


                glm::mat4 mvp = game->calculateMvpMatrix(glm::vec3(bodyCenter, position.z), angle,

                                                         glm::vec3(bodyWidth, laserThickness, 1.0f));


                laserBodySprite->draw(*shader, mvp);

                debugBodyDrawCount++;
            }
        }


        bool headDrawn = false; // Track if we've placed the origin head


        bool tailDrawn = false; // Add this before your loop starts
        // --- PASS 2: DRAW CAPS (Origin, Junctions, Terminals) ---

        glm::vec2 lastCornerPos = glm::vec2(-999.0f, -999.0f);

        for (size_t i = 0; i < pointsSnapshot.size() - 1; ++i) {
            glm::vec2 start = pointsSnapshot[i];


            glm::vec2 end = pointsSnapshot[i + 1];


            glm::vec2 dir = glm::normalize(end - start);

            if (i == 0) {
                float fDir = (dir.x > 0.0f) ? -1.0f : 1.0f;


                // 1. Calculate the scale as a vector


                glm::vec3 currentScale = glm::vec3(headWidth * fDir, laserThickness - 0.09f, 1.0f);


                // 2. Pass this scale directly into your MVP calculation (do not call setScale)


                glm::mat4 mvp = game->calculateMvpMatrix(


                    glm::vec3(start + (dir * (headWidth * 0.35f)), position.z),


                    0.0f,


                    currentScale // Pass the scale here


                );


                laserHeadSprite->draw(*shader, mvp);


                headDrawn = true;

                //
                //
                // std::cout << "HEAD dir.x=" << dir.x << " fDir=" << fDir << std::endl;
                continue;
            }
            int interaction = mirrorSnapshot[i - 1];


            float angle = std::atan2(dir.y, dir.x) * (180.0f / 3.14159f);

            // 2. DRAW JUNCTION


            auto safeMarkDrawn = [&](int index) {
                if (index >= 0 && index < (int) cornerDrawn.size()) {
                    cornerDrawn[index] = true;
                }
            };
            // To this:


            //if (i == 0 || i >= mirrorSnapshot.size()) continue;

            //
            // std::cout << " i ist right now: " << i << std::endl;


            if (i < mirrorSnapshot.size() && mirrorSnapshot[i - 1] != (int) MirrorInteraction::None) {
                // bool isReturning = (pointsSnapshot.size() > 2) && (i == 1) &&

                //

                // (glm::distance(pointsSnapshot[2], pointsSnapshot[0]) < 0.2f);


                glm::vec2 incomingDir = glm::normalize(pointsSnapshot[i] - pointsSnapshot[i - 1]);


                float fDir = (incomingDir.x > 0.0f) ? 1.0f : -1.0f;
                // 1. Define a tolerance
                const float ANGLE_TOLERANCE = 5.0f; // 5 degrees of leeway
                //bool isReturning = (glm::dot(incomingDir, dir) < -0.9f); // Uses the dot product (more robust)

                float segmentAngle = std::atan2(dir.y, dir.x) * (180.0f / 3.14159f);
                // std::cout <<"RAW SEGMENT ANGLE: " << segmentAngle << std::endl;
                float finalAngle = segmentAngle; // Default

                if (interaction == (int) MirrorInteraction::Reversal) {
                    // std::cout << "WERE ENTERING THE IF!!" << std::endl;
                    // std::cout << "segmentAngle: " << segmentAngle << std::endl;

                    // Use range checks for stability
                    if (std::abs(segmentAngle - 180.0f) < ANGLE_TOLERANCE) {
                        finalAngle = 0.0f; // Override if it's "effectively" 180
                    } else if (segmentAngle < -85.0f && segmentAngle > -95.0f) {
                        // std::cout << "SEGMENT ANGLE IS IN THE -90 range!" << std::endl;
                        finalAngle = 90.0f; // Override
                    }

                    // DRAW ONLY ONCE
                    glm::mat4 mvp = game->calculateMvpMatrix(
                        glm::vec3(start + (dir * (headWidth * 0.35f)), position.z),
                        finalAngle,
                        glm::vec3(headWidth * fDir, laserThickness - 0.09f, 1.0f)
                    );
                    laserHeadSprite->draw(*shader, mvp);
                    safeMarkDrawn(i + 1);
                    safeMarkDrawn(i + 2);
                }


                if (interaction == (int) MirrorInteraction::Turn && !cornerDrawn[i]) {
                    //&& mirrorSnapshot[i] != (int)

                    //MirrorInteraction::Reversal


                    glm::vec2 incomingD = segmentDirections[i - 1];

                    // Check if the current segment 'i' is the same direction as the original path

                    // If dotProduct with original path is negative, we are on the return trip


                    float pipeRotation = -1.0f;

                    MirrorCube *cube = segmentMirrorCubes[i - 1];

                    if (cube != nullptr) {
                        pipeRotation = cube->getPipeRotation(); // Always gets the latest 135!
                    }


                    if (!laserHeadSprite || !laserTailSprite || !laserBodySprite || !laserCornerSprite || !shader)


                        return;


                    if (animFrame >= headFrames.size()) return; // Protect frame indexing


                    float cornerAngle = 0.0f;


                    float scaleX = headWidth / 1.5f;


                    float scaleY = headWidth / 1.5f;


                    // Use a threshold (0.5f) so the code ignores tiny math errors


                    bool incomingFromRight = incomingD.x < -0.5f;


                    bool incomingFromLeft = incomingD.x > 0.5f;


                    bool incomingFromDown = incomingD.y > 0.5f;


                    bool incomingFromUp = incomingD.y < -0.5f;


                    // 1. Get the rotation into the 0-360 range


                    float rot = std::fmod(pipeRotation, 360.0f);


                    if (rot < 0.0f) rot += 360.0f;


                    // 2. Map rotation to an integer index (0, 1, 2, 3)


                    // 45 degrees is index 0, 135 is index 1, etc.


                    int rotIndex = (int) (rot / 90.0f) % 4;


                    glm::vec2 incomingDi = segmentDirections[i - 1];


                    // Calculate 'dir' (outgoing) from your existing pointsSnapshot


                    glm::vec2 outgoingD = glm::normalize(pointsSnapshot[i + 1] - pointsSnapshot[i]);


                    // NOW calculate your dotProduct using these two local variables


                    float dotProductt = glm::dot(incomingD, outgoingD);


                    float cornerOffsetX = 0.0f;


                    float cornerOffsetY = 0.0f;


                    // 2. Clear Logic Blocks

                    std::cout << "PIPE ROTATION: " << pipeRotation << std::endl;


                    if (incomingFromRight && !incomingFromDown && !incomingFromUp) {
                        // Laser traveling left (incomingDir.x < 0)


                        if (rotIndex == 0 || rotIndex == 2) {
                            // Mapping 45°


                            cornerAngle = 180.0f;


                            cornerOffsetX = 0.077f;

                            cornerOffsetY = -0.08f;


                            std::cout << "INCOMIG RIGHT 45" << std::endl;
                        } else if (rotIndex == 1 || rotIndex == 3) {
                            // Mapping 135°


                            cornerAngle = 270.0f;


                            cornerOffsetX = 0.077f;

                            cornerOffsetY = 0.08f;


                            std::cout << "INCOMIG RIGHT 135" << std::endl;
                        }


                        // std::cout << "WERE COMING FROM THE RIGHT!!!!" << std::endl;
                    } else if (incomingFromLeft && !incomingFromDown && !incomingFromUp) {
                        // Laser traveling right (incomingDir.x > 0)


                        if (rotIndex == 0 || rotIndex == 2) {
                            // Mapping 45°


                            cornerAngle = 0.0f;


                            cornerOffsetX = -0.085f;

                            cornerOffsetY = 0.088f;


                            std::cout << "INCOMING LEFT FROM 45 " << std::endl;
                        } else if (rotIndex == 1 || rotIndex == 3) {
                            // Mapping 135°


                            cornerAngle = 90.0f;


                            cornerOffsetX = -0.077f;

                            cornerOffsetY = -0.08f;


                            std::cout << "INCOMING LEFT FROM 135 " << std::endl;
                        }


                        // std::cout << "WERE COMING FROM THE LEFT!!!!" << std::endl;
                    } else if (incomingFromDown && !incomingFromLeft && !incomingFromRight) {
                        if (rotIndex == 0 || rotIndex == 2) {
                            cornerAngle = 180.0f;


                            cornerOffsetX = 0.077f;

                            cornerOffsetY = -0.08f;


                            std::cout << "ICOMING DOWN FROM 45 " << std::endl;
                        } else if (rotIndex == 1 || rotIndex == 3) {
                            // Mapping 135°


                            cornerAngle = 90.0f;


                            cornerOffsetX = -0.077f;

                            cornerOffsetY = -0.08f;


                            std::cout << "ICOMING DOWN FROM 135 " << std::endl;
                        }
                    } else if (incomingFromUp && !incomingFromLeft && !incomingFromRight) {
                        if (rotIndex == 0 || rotIndex == 2) {
                            cornerAngle = 270.0f;


                            cornerOffsetX = 0.077f;

                            cornerOffsetY = 0.08f;


                            std::cout << "ICOMING UP FROM 45 DEGREES" << std::endl;
                        } else if (rotIndex == 1 || rotIndex == 3) {
                            // Mapping 135°


                            cornerAngle = 0.0f;


                            cornerOffsetX = -0.085f;

                            cornerOffsetY = 0.088f;


                            std::cout << "ICOMING UP FROM 135 DEGREES" << std::endl;
                        }
                    }


                    // std::cout << "Actually drawing corner sprite at i=" << i << " incomingAngle=" << cornerAngle <<


                    //


                    // " outgoingAngle=" << angle << " cornerAngle=" << cornerAngle << std::endl;


                    // Dot product to ensure we are only drawing forward motion

                    // This stops the "return path" double-drawing

                    float pathProgress = glm::dot(incomingD, outgoingD);


                    // Filter out straight lines (dot 1.0) and reversals (dot -1.0)

                    // Only draw if we are actually turning (~0.0)

                    if (std::abs(pathProgress) < 0.5f) {
                        glm::mat4 mvp = game->calculateMvpMatrix(


                            glm::vec3(start.x + cornerOffsetX, start.y + cornerOffsetY, position.z),


                            cornerAngle,


                            glm::vec3(scaleX, scaleY, 1.0f)


                        );


                        laserCornerSprite->draw(*shader, mvp);

                        // Mark this index as processed

                        cornerDrawn[i] = true;
                    }

                    //continue;
                }


                // 3. DRAW TERMINAL (Only the very last point)
            }
        }


        if
        (!mirrorSnapshot.empty() && mirrorSnapshot.back() == (int)MirrorInteraction::Hit) {
            glm::vec2 end = pointsSnapshot.back();
            glm::vec2 prev = pointsSnapshot[pointsSnapshot.size() - 2];
            glm::vec2 dir = glm::normalize(end - prev);

            float rotation = 0.0f;
            float offset = 0.18f; // Adjust this to push the tail flush against the wall
            glm::vec2 finalPos = end;

            // Check direction and set rotation + position
            if (dir.x < -0.5f) {
                // Incoming from Right (Moving Left)
                rotation = 0.0f;
                finalPos.x += offset;
            } else if (dir.x > 0.5f) {
                // Incoming from Left (Moving Right)
                rotation = 180.0f;
                finalPos.x -= offset;
            } else if (dir.y < -0.5f) {
                // Incoming from Up (Moving Down)
                rotation = 90.0f;
                finalPos.y += offset;
            } else if (dir.y > 0.5f) {
                // Incoming from Down (Moving Up)
                rotation = 270.0f;
                finalPos.y -= offset;
            }

            glm::mat4 mvp = game->calculateMvpMatrix(
                glm::vec3(finalPos, position.z),
                rotation,
                glm::vec3(tailWidth, laserThickness * 3.0f, 1.0f)
            );

            laserTailSprite->draw(*shader, mvp);
        }

        if
        (debugBodyDrawCount > 1) {
            std::cout << "[DEBUG] TOTAL BODIES DRAWN: " << debugBodyDrawCount << std::endl;
        }
    }


void LaserGuy::update(Game *game, float deltaTime) {
    Entity::update(game, deltaTime);

    if (isDying) {
        deathTimer += deltaTime;
        float fadeDuration = 0.5f;
        float alpha = (std::max)(0.0f, 1.0f - (deathTimer / fadeDuration));
        if (!deathSoundPlayed) {
            laserSoundHandle = game->PlaySfxWithHandle(game->getLaserGuyHitSFX(), 0.5f);
            deathSoundPlayed = true;
        }


        // Apply alpha to sprite
        glm::vec4 currentColor = sprite->getColor();
        sprite->setColor(glm::vec4(currentColor.r, currentColor.g, currentColor.b, alpha));

        // When fade finishes, kill him
        if (deathTimer >= fadeDuration) {
            isVisible = false;
            game->StopSFX(laserSoundHandle); // fade out and stop



            // Disable physics immediately so he doesn't block anything while fading
            if (b2Body_IsValid(body)) {
                b2DestroyBody(body);
                body = b2_nullBodyId;
            }
            shouldBeRemoved = true; // Tell Scene to delete me
        }
        return; // Don't run the rest of the update/raycasting logic while dying
    }

    // 2. Then tick the animation based on that path
    // 1. Tick down your master state loop clock
    stateTimer += deltaTime;

        if (currentState == LaserState::idle) {
            if (stateTimer >= idleDuration) {
                currentState = LaserState::firing;
                stateTimer = 0.0f;
                animFrame = 0;
                animTimer = 0.0f;
                laserSoundHandle = game->PlaySfxWithHandle(game->getLaserFiredSFX(), 0.15f);
            }
        } else if (currentState == LaserState::firing) {
            castRay();
            // REMOVE game->PlaySfx from here
            isRaycastReady = true;
            updateAnimation(deltaTime);

            if (stateTimer >= firingDuration) {
                currentState = LaserState::idle;
                stateTimer = 0.0f;
                guyAnimFrame = 0;
                game->StopSFX(laserSoundHandle); // fade out and stop
            }
        }
    bool currentlyPassingThrough = false;
    for (int interaction: segmentEndsOnMirror) {
        if (interaction == (int) MirrorInteraction::PassThrough) {
            currentlyPassingThrough = true;
            break;
        }
    }

    // Dynamic Swap
    if (currentlyPassingThrough) {
        this->setZOrder(0); // Set lower than the Pipe's Z-order (e.g., 1)
    } else {
        this->setZOrder(2); // Set higher than the Pipe's Z-order
    }
}


void LaserGuy::createPhysicsBody() {
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.enableSleep = false; // Prevent body from sleeping
    bodyDef.type = b2_kinematicBody;
    bodyDef.position = {position.x, position.y};
    bodyDef.userData = this;
    body = b2CreateBody(physicsWorld, &bodyDef);

    // 1. Define dimensions
    HalfWidth = abs(scale.x / 2.0f);
    HalfHeight = abs(scale.y / 2.0f);

    // 2. Create a standard box instead of a complex hull
    b2Polygon box = b2MakeBox(HalfWidth, HalfHeight);

    // 3. Create the shape
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.userData = this;
    shapeDef.filter.categoryBits = gl3::PhysicsCategories::CATEGORY_LASER_EYES;

    shape = b2CreatePolygonShape(body, &shapeDef, &box);
    b2Shape_EnableSensorEvents(shape, true);
    b2Shape_SetUserData(shape, this);
}

void LaserGuy::castRay() {
    b2Vec2 guyPos = b2Body_GetPosition(body);

    float spriteDir = sprite->getScale().x > 0.0f ? 1.0f : -1.0f;


    glm::vec2 currentOrigin = glm::vec2(guyPos.x + (spriteDir * eyeOffsetX), guyPos.y + eyeOffsetY);

    glm::vec2 currentDir = glm::vec2(spriteDir, 0.0f);
    // this is JUST FOR THE INITIAL DIRECTION COMING FROM LASERGUY


    hitEntities.clear();

    laserPoints.clear();

    segmentEndsOnMirror.clear();
    segmentMirrorCubes.clear();

    laserPoints.push_back(currentOrigin);


    uint64_t mask = gl3::PhysicsCategories::CATEGORY_PLAYER |

                    gl3::PhysicsCategories::CATEGORY_WALL |

                    gl3::PhysicsCategories::CATEGORY_DOOR |

                    gl3::PhysicsCategories::CATEGORY_MIRROR |

                    gl3::PhysicsCategories::CATEGORY_MIRROR_PIPE |

                    gl3::PhysicsCategories::CATEGORY_LASER_EYES |

                    gl3::PhysicsCategories::CATEGORY_BOX |

                    gl3::PhysicsCategories::CATEGORY_FLOOR;


    for (int bounce = 0; bounce < 10; ++bounce) {
        glm::vec2 rayEnd = currentOrigin + (currentDir * 10.0f);

        RayCastResult result = PhysicsUtils::CastRay(physicsWorld,

                                                     {currentOrigin.x, currentOrigin.y},

                                                     {rayEnd.x, rayEnd.y},

                                                     mask,

                                                     gl3::PhysicsCategories::CATEGORY_RAY_SENSOR);


        // 1. NO HIT: Terminate

        if (!result.didHit) {
            laserPoints.push_back(rayEnd);
            // !!! ADD THIS LINE !!!
            segmentMirrorCubes.push_back(nullptr);

            segmentEndsOnMirror.push_back((int) MirrorInteraction::None);

            break;
        }


        Entity *hitEntity = static_cast<Entity *>(b2Shape_GetUserData(result.shapeId));

        glm::vec2 hitPoint = glm::vec2(result.hitPoint.x, result.hitPoint.y);


        // 2. SELF-HIT FILTER

        if (hitEntity == this && bounce == 0) {
            currentOrigin = currentOrigin + (currentDir * 0.2f);

            continue;
        }


        // 3. NUCLEAR OPTION (Distance Debounce)

        if (!laserPoints.empty() && glm::distance(laserPoints.back(), hitPoint) < 0.05f) {
            currentOrigin = hitPoint + (currentDir * 0.1f);

            continue;
        }


        segmentDirections.push_back(currentDir);

        // 4. MIRROR REFLECTION / PASS-THROUGH

        // Inside your loop
        if (hitEntity && (hitEntity->getType() == EntityType::Mirror || hitEntity->getType() ==
                          EntityType::MirrorCube)) {
            if (hitEntity->getType() == EntityType::MirrorCube) {
                segmentMirrorCubes.push_back(dynamic_cast<MirrorCube *>(hitEntity));
            }

            if (hitEntity->getType() == EntityType::MirrorCube && isParallelToPipe(
                    currentDir, (MirrorCube *) hitEntity)) {
                // std::cout << "PASSTHROUGH HIT on entity at: " << hitEntity->getPosition().x << "," << hitEntity->getPosition().y << std::endl;


                currentOrigin = hitPoint + (glm::normalize(currentDir) * 0.5f);

                laserPoints.push_back(hitPoint);

                segmentEndsOnMirror.push_back((int) MirrorInteraction::PassThrough);
            } else {
                glm::vec2 normal = glm::vec2(result.normal.x, result.normal.y);


                // 1. Capture the direction BEFORE reflection for the dot product

                glm::vec2 oldDir = currentDir;


                // 2. Calculate the new direction

                glm::vec2 reflectionVector = glm::reflect(oldDir, normal);


                // 3. Determine Interaction (Use old vs new)

                float dotProduct = glm::dot(glm::normalize(oldDir), glm::normalize(reflectionVector));


                if (dotProduct < -0.8f) {
                    segmentEndsOnMirror.push_back((int) MirrorInteraction::Reversal);

                    // std::cout << "REFLECTION VECTOR: " << reflectionVector.x << "," << reflectionVector.y << std::endl;
                } else if (std::abs(dotProduct) < 0.5f) {
                    segmentEndsOnMirror.push_back((int) MirrorInteraction::Turn);

                    // std::cout << "REFLECTION VECTOR: " << reflectionVector.x << "," << reflectionVector.y << std::endl;

                    incoming = spriteDir;

                    // std::cout << "OLDDIR IS: " << incoming << std::endl;

                    reflected = reflectionVector;

                    // std::cout << "NEW DIR IS: " << reflected.x << "," << reflected.y << std::endl;
                } else {
                    segmentEndsOnMirror.push_back((int) MirrorInteraction::None);
                }


                currentDir = reflectionVector;

                currentOrigin = hitPoint + (currentDir * 0.05f);

                laserPoints.push_back(hitPoint);
            }
        }

        // 5. WALL HIT

        else if (hitEntity->getType() == EntityType::LaserGuy) {
            // I THINK THIS IS OKAY BUT MAYBE DOUBLE CHECK LATER

            segmentEndsOnMirror.push_back((int) MirrorInteraction::None);

            laserPoints.push_back(hitPoint);
            segmentMirrorCubes.push_back(nullptr);

            std::cout << "LASERGUY HIT" << std::endl;

            isDying = true;

            break;
        } else if (hitEntity->getType() == EntityType::Player) {
            Ship *hitship = dynamic_cast<Ship *>(hitEntity);

            segmentEndsOnMirror.push_back((int) MirrorInteraction::Hit);

            laserPoints.push_back(hitPoint);
            segmentMirrorCubes.push_back(nullptr);

            std::cout << "we are hitting player goddammit" << std::endl;

            hitship->setIsDead(true);

            break;
        } else if (hitEntity->getType() == EntityType::Wall || hitEntity->getType() == EntityType::Box || hitEntity
                   ->getType() == EntityType::Door || hitEntity->getType() == EntityType::Floor) {
            segmentEndsOnMirror.push_back((int) MirrorInteraction::Hit);
            segmentMirrorCubes.push_back(nullptr);

            laserPoints.push_back(hitPoint);


            break;
        }
    }
}


// else {
//
//         laserPoints.push_back(hitPoint);
//     segmentEndsOnMirror.push_back((int)MirrorInteraction::Hit);
//
//     // std::cout << "for some reason we are not hitting the player" << std::endl;
//     // ADD THIS DEBUG
//     if (hitEntity) {
//         // std::cout << "DEBUG: Hit entity type ID is: " << (int)hitEntity->getType() << std::endl;
//     } else {
//         std::cout << "DEBUG: Hit entity is NULL, but result.didHit was TRUE!" << std::endl;
//     }
//     }


// std::cout << "Normal: " << result.normal.x << "," << result.normal.y << std::endl;

bool LaserGuy::isParallelToPipe(glm::vec2 laserDir, MirrorCube *pipe) {
    // This now compares the vector magnitude correctly on both axes
    float pipeAngle = glm::radians(pipe->getPipeRotation());
    glm::vec2 pipeDir = {std::cos(pipeAngle), std::sin(pipeAngle)};

    // The dot product naturally handles both X and Y components simultaneously
    float dot = std::abs(glm::dot(glm::normalize(laserDir), pipeDir));
    return (dot > 0.95f);
}

void LaserGuy::updateAnimation(float deltaTime) {
    animTimer += deltaTime;

    if (animTimer >= frameDuration) {
        animTimer = 0.0f; // Reset the mini-timer for the next frame

        // Advance to next frame index (0 -> 1 -> 2 -> 3 -> 0)
        animFrame = (animFrame + 1) % totalLaserFrames;
        guyAnimFrame = (guyAnimFrame + 1) % totalGuyFrames;
    }
}

MirrorCube *LaserGuy::getCubeAtSegment(int i) {
    // Safety check: is the index valid?
    if (i < 0 || i >= hitEntities.size()) return nullptr;

    Entity *e = hitEntities[i];

    // Check if the entity exists and is a MirrorCube
    if (e && e->getType() == EntityType::MirrorCube) {
        return static_cast<MirrorCube *>(e);
    }

    return nullptr;
}

bool LaserGuy::isConfigReflective(glm::vec2 laserDir, MirrorCube *cube) {
    // 1. Get the direction the pipe is facing
    float rotRad = glm::radians(cube->getPipeRotation());
    glm::vec2 pipeDir = glm::vec2(std::cos(rotRad), std::sin(rotRad));

    // 2. Normalize both directions
    glm::vec2 normLaser = glm::normalize(laserDir);
    glm::vec2 normPipe = glm::normalize(pipeDir);

    // 3. Dot product:
    // If dot is near 0, the vectors are perpendicular (Orthogonal)
    // If dot is near 1 or -1, the vectors are parallel (the laser is going into the hole)
    float dot = std::abs(glm::dot(normLaser, normPipe));

    // If dot is less than 0.1, the laser is hitting the side -> ACT AS WALL
    if (dot < 0.1f) {
        return false;
    }

    // Otherwise, the laser is entering the "hole" (parallel-ish) -> REFLECT
    return true;
}

}
