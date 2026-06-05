#include "LaserGuy.h"
#include "../physics/PhysicsUtils.h"
#include "../PhysicsCategories.h"
#include "../rendering/Texture.h"
#include "box2d/box2d.h"
#include "box2d/src/body.h"
#include <iostream>d


namespace gl3 {
    LaserGuy::LaserGuy(Shader *shader, glm::vec3 position, glm::vec3 scale, b2WorldId physicsWorld) :
    Entity(shader,
        position,
        scale,
        physicsWorld,
        EntityType::LaserGuy)
    {
        this->sprite = new Sprite(position, glm::vec3 (-scale.x,scale.y, scale.z), Texture::LoadTexture("assets/textures/temp_laserEyes.png"));
        this->setZOrder(2);
        // 1. Allocate the 3 separate laser piece sprites
        this->laserHeadSprite = new Sprite(position, scale, 0);
        this->laserBodySprite = new Sprite(position, scale, 0);
        this->laserTailSprite = new Sprite(position, scale, 0);

        // 2. Load Head Animation Frames
        headFrames.push_back(Texture::LoadTexture("assets/textures/laser_head_1.png"));
        headFrames.push_back(Texture::LoadTexture("assets/textures/laser_head_2.png"));
        headFrames.push_back(Texture::LoadTexture("assets/textures/laser_head_3.png"));
        headFrames.push_back(Texture::LoadTexture("assets/textures/laser_head_4.png"));
        headFrames.push_back(Texture::LoadTexture("assets/textures/laser_head_5.png"));
        headFrames.push_back(Texture::LoadTexture("assets/textures/laser_head_6.png"));

        // 3. Load Body Animation Frames
        bodyFrames.push_back(Texture::LoadTexture("assets/textures/laser_body_1.png"));
        bodyFrames.push_back(Texture::LoadTexture("assets/textures/laser_body_2.png"));
        bodyFrames.push_back(Texture::LoadTexture("assets/textures/laser_body_3.png"));
        bodyFrames.push_back(Texture::LoadTexture("assets/textures/laser_body_4.png"));
        bodyFrames.push_back(Texture::LoadTexture("assets/textures/laser_body_5.png"));
        bodyFrames.push_back(Texture::LoadTexture("assets/textures/laser_body_6.png"));

        // 4. Load Tail Animation Frames
        tailFrames.push_back(Texture::LoadTexture("assets/textures/laser_end_1.png"));
        tailFrames.push_back(Texture::LoadTexture("assets/textures/laser_end_2.png"));
        tailFrames.push_back(Texture::LoadTexture("assets/textures/laser_end_3.png"));
        tailFrames.push_back(Texture::LoadTexture("assets/textures/laser_end_4.png"));
        tailFrames.push_back(Texture::LoadTexture("assets/textures/laser_end_5.png"));
        tailFrames.push_back(Texture::LoadTexture("assets/textures/laser_end_6.png"));
        LaserGuy::createPhysicsBody();

    }
 void LaserGuy::draw(Game *game) {
    // 1. Always draw LaserGuy base asset first
    Entity::draw(game);

    // 2. If firing, render the synchronized 3-piece laser
    if (currentState == LaserState::firing) {
        this->laserHeadSprite->setTexture(headFrames[animFrame]);
        this->laserBodySprite->setTexture(bodyFrames[animFrame]);
        this->laserTailSprite->setTexture(tailFrames[animFrame]);

        // --- Align visual origin perfectly with your raycast eye origin ---
        float spriteDir = sprite->getScale().x > 0.0f ? 1.0f : -1.0f;
        glm::vec3 eyeOrigin = glm::vec3(
            this->position.x + (spriteDir * eyeOffsetX),
            this->position.y + eyeOffsetY,
            this->position.z
        );

        float startX = eyeOrigin.x;
        float endX = currentHitPoint.x;
        float totalDistance = std::abs(endX - startX);
        float direction = (endX > startX) ? 1.0f : -1.0f;

        // --- UNIFORM LASER DIMENSIONS ---
        float headWidth = 0.4f;
        float tailWidth = 0.4f;
        float bodyWidth = totalDistance - (headWidth + tailWidth);
        if (bodyWidth < 0.0f) bodyWidth = 0.0f;

        // NEW: Force every single piece to use this exact thickness instead of scale.y!
        float laserThickness = 0.2f; // Adjust this down (e.g., 0.1f) to make the whole beam thinner

        // --- DRAW HEAD ---
        glm::vec3 headPos = glm::vec3(startX + (direction * (headWidth / 2.0f)), eyeOrigin.y, eyeOrigin.z);
        glm::vec3 headScale = glm::vec3(headWidth * -direction, laserThickness -0.07f, 1.0f); // <-- Fixed thickness
        glm::mat4 headMvp = game->calculateMvpMatrix(headPos, 0.0f, headScale);
        laserHeadSprite->draw(*shader, headMvp);

        // --- DRAW BODY ---
        glm::vec3 bodyPos = glm::vec3(startX  + 0.0075f +(direction * (headWidth + (bodyWidth / 2.0f))), eyeOrigin.y, eyeOrigin.z);
        glm::vec3 bodyScale = glm::vec3(bodyWidth * -direction, laserThickness, 1.0f); // <-- Fixed thickness
        glm::mat4 bodyMvp = game->calculateMvpMatrix(bodyPos, 0.0f, bodyScale);
        laserBodySprite->draw(*shader, bodyMvp);

        // --- DRAW TAIL ---
        glm::vec3 tailPos = glm::vec3(endX + 0.73f +(direction * (tailWidth / 2.0f)), eyeOrigin.y, eyeOrigin.z);
        glm::vec3 tailScale = glm::vec3(tailWidth * -direction, laserThickness + 0.5f, 1.0f); // <-- Fixed thickness
        glm::mat4 tailMvp = game->calculateMvpMatrix(tailPos, 0.0f, tailScale);
        laserTailSprite->draw(*shader, tailMvp);
    }
}
    void LaserGuy::update(Game *game, float deltaTime) {
        Entity::update(game, deltaTime);

        // 1. Tick down your master state loop clock
        stateTimer += deltaTime;

        if (currentState == LaserState::idle) {
            if (stateTimer >= idleDuration) {
                currentState = LaserState::firing;
                stateTimer = 0.0f; // Reset timer for the firing phase

                // CRUCIAL: Reset animation tracking values when firing starts
                animFrame = 0;
                animTimer = 0.0f;
            }
        }
        else if (currentState == LaserState::firing) {
            castRay(); // Triggers castRay()

            // 2. HERE IS WHERE THE ANIMATION TICK HAPPENS!
            // This keeps animFrame incrementing frame-by-frame while firing
            updateAnimation(deltaTime);

            if (stateTimer >= firingDuration) {
                currentState = LaserState::idle;
                stateTimer = 0.0f; // Reset timer for the idle phase
            }
        }
    }



    void LaserGuy::createPhysicsBody() {
        b2BodyDef bodyDef = b2DefaultBodyDef();
        bodyDef.enableSleep = false; // Prevent body from sleeping
        bodyDef.type = b2_staticBody;
        bodyDef.position = {position.x, position.y};
        bodyDef.userData = this;
        body = b2CreateBody(physicsWorld, &bodyDef);

        // 1. Define dimensions
        HalfWidth = scale.x / 2.0f;
        HalfHeight = scale.y / 2.0f;

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

        // 1. Determine which way he is facing
        float spriteDir = sprite->getScale().x > 0.0f ? 1.0f : -1.0f;

        // 2. Shift the ray origin dynamically based on direction!
        b2Vec2 origin = {
            guyPos.x + (spriteDir * eyeOffsetX), // Moves left or right depending on face direction
            guyPos.y + eyeOffsetY              // Moves up to eye level
        };

        // 3. Project outward from the new eye position
        b2Vec2 end = {origin.x + (spriteDir * 10.0f), origin.y};

        uint64_t category = gl3::PhysicsCategories::CATEGORY_RAY_SENSOR;
        uint64_t mask = gl3::PhysicsCategories::CATEGORY_PLAYER |
                        gl3::PhysicsCategories::CATEGORY_WALL |
                        gl3::PhysicsCategories::CATEGORY_DOOR |
                        gl3::PhysicsCategories::CATEGORY_BOX;

        RayCastResult result = PhysicsUtils::CastRay(physicsWorld, origin, end, mask, category);
        if (result.didHit) {
            currentHitPoint = glm::vec2(result.hitPoint.x, result.hitPoint.y);

            // ADD THIS TEMPORARY DEBUG PRINT:
            b2ShapeId hitShape = result.shapeId; // Assuming your CastRay returns the shape ID
            void* userData = b2Shape_GetUserData(hitShape);
            std::cout << "LASER HIT POINT: " << result.hitPoint.x << ", " << result.hitPoint.y << std::endl;
        }
    }
    void LaserGuy::updateAnimation(float deltaTime) {
        animTimer += deltaTime;

        if (animTimer >= frameDuration) {
            animTimer = 0.0f; // Reset the mini-timer for the next frame

            // Advance to next frame index (0 -> 1 -> 2 -> 3 -> 0)
            animFrame = (animFrame + 1) % totalLaserFrames;
        }
    }



}
