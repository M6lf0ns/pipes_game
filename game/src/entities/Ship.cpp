#include "Ship.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <iostream>

#include "../Game.h"
#include "../Assets.h"
#include "../rendering/Texture.h"
#include "glm/gtx/string_cast.hpp"


namespace gl3 {

    bool gl3::Ship::push_PullKeyPressed = false;
    bool gl3::Ship::spaceWasPressed = false;
    float gl3::Ship::shipHalfHeight;
    float gl3::Ship::playerVelX;
    glm::vec3 gl3::Ship::playerPos;

    Ship::Ship(glm::vec3 position, float zRotation, glm::vec3 scale, b2WorldId physicsWorld) : Entity(
            Shader("shaders/vertexShader.vert", "shaders/fragmentShader.frag"),
            position,
            zRotation,
            scale,
            {1, 1, 1, 1},
            physicsWorld,
            EntityType::Player),
    sprite(position, scale, Texture::LoadTexture("assets/textures/Awesome_face.png")) // <— initialize sprite here
    {
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

        auto body = getBody();

        float maxSpeed = 1.0f;

        vel = b2Body_GetLinearVelocity(body); //get current velocity

        // Horizontal movement
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            vel.x = -translationSpeed; // move left
            if (vel.x < -maxSpeed) vel.x = -maxSpeed;
            b2Body_SetLinearVelocity(body, vel);
            //position.x -= translationSpeed * deltaTime;


        } else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            vel.x = translationSpeed;  // move right
            // Clamp horizontal velocity
            if (vel.x > maxSpeed) vel.x = maxSpeed;
            b2Body_SetLinearVelocity(body, vel);
            //position.x += translationSpeed * deltaTime; This would manually teleport the sprite instead of relying on UpdateBasedOnPhysics
        } // if neither A nor D pressed, x stays 0

        else {
            vel.x = 0.0f;

        }
        // Flip sprite horizontally based on movement direction
        if (vel.x > 0.0f) { //velocity positive --> sprite faces right
            sprite.setScale(glm::vec3(abs(sprite.getScale().x), sprite.getScale().y, sprite.getScale().z));
        } else if (vel.x < 0.0f) { //velocity negative --> sprite faces left
            sprite.setScale(glm::vec3(-abs(sprite.getScale().x), sprite.getScale().y, sprite.getScale().z));
        }

        bool spacePressed = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
        //vertical movement
        if(spacePressed && !spaceWasPressed) {
            b2Body_ApplyLinearImpulseToCenter(body, {0.0f, 5.0f}, true);
        }
        spaceWasPressed = spacePressed;


        // E key for pushing
        push_PullKeyPressed = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;

        playerVelX = vel.x;

        playerPos = position;

        updateBasedOnPhysics();

        // Prevent any rotation
        b2Body_SetAngularVelocity(body, 0);

    }



    void Ship::draw(Game *game) {

        // Debug: check rendered sprite bounds
        glm::vec3 spritePos = sprite.getPosition();
        glm::vec3 spriteScale = sprite.getScale();

        float spriteBottom = spritePos.y - spriteScale.y / 2.0f;
        float spriteTop = spritePos.y + spriteScale.y / 2.0f;

        // std::cout << "Sprite bounds - Bottom: " << spriteBottom << " Top: " << spriteTop << std::endl;
        // std::cout << "Sprite position Y: " << spritePos.y << " Scale Y: " << spriteScale.y << std::endl;

        sprite.draw(shader);
        // Debug: print the MVP matrix
        // debug print only once
        static bool printed = false;
        if (!printed) {
            glm::mat4 mvp = game->calculateMvpMatrix(sprite.getPosition(), 0.0f, sprite.getScale());
            std::cout << "Ship MVP:\n" << glm::to_string(mvp) << std::endl;
            printed = true;  // ensures this block never runs again
        }



    }

    void Ship::createPhysicsBody()
    {
        b2BodyDef bodyDef = b2DefaultBodyDef();
        bodyDef.type = b2_dynamicBody;
        bodyDef.position = {position.x, position.y};
        bodyDef.userData = this;
        body = b2CreateBody(physicsWorld, &bodyDef);

        b2ShapeDef shapeDef = b2DefaultShapeDef();

        // Use b2MakeBox with the correct half-extents
        float halfWidth = 0.125f;  // 0.25f / 2
        float halfHeight = 0.125f; // 0.25f / 2

        b2Polygon box = b2MakeBox(halfWidth, halfHeight);
        shape = b2CreatePolygonShape(body, &shapeDef, &box);

        // Debug
        b2AABB aabb = b2Shape_GetAABB(shape);
        std::cout << "Fixed Ship AABB: " << (aabb.upperBound.x - aabb.lowerBound.x)
                  << " x " << (aabb.upperBound.y - aabb.lowerBound.y) << std::endl;

        shipHalfHeight = halfHeight;

        std::cout << "AABB Dimensions: " << (aabb.upperBound.x - aabb.lowerBound.x)
                  << " x " << (aabb.upperBound.y - aabb.lowerBound.y) << std::endl;
        std::cout << "Expected dimensions: 0.25 x 0.25" << std::endl;

        // Also check the body transform
        auto transform = b2Body_GetTransform(body);
        std::cout << "Body transform - position: (" << transform.p.x << ", " << transform.p.y
                  << ") rotation: " << transform.q.s << std::endl;

        b2MassData massData;
        massData.mass = 1.5f;
        massData.center = b2Vec2{0.0f, 0.0f};
        massData.rotationalInertia = 0.0f; // must be explicitly set
        b2Body_SetMassData(body, massData);
        b2Body_SetLinearDamping(body, 5.0f); // tweak this value
    }

    void Ship::updateBasedOnPhysics()
    {
        Entity::updateBasedOnPhysics();
        //setting the sprite´s position, can be avoided if we add Sprite to Entity
        //would not need to be overriden anymore
        b2Vec2 bodyPos = b2Body_GetPosition(body);
        position.x = bodyPos.x;
        position.y = bodyPos.y;
        sprite.setPosition(position);





    }
    void Ship::setPosition(const glm::vec3 &newPosition) {
        Entity::setPosition(newPosition);


        // Update physics body
        if (body.index1 != 0) {  // Check if body is valid
            b2Body_SetTransform(body, b2Vec2{newPosition.x, newPosition.y}, b2Body_GetRotation(body)); //already sets sprite on the ground,if not, sprite falls onto ground
        }
    }





    void Ship::startContact() {
        std::cout << "Ship start contact" << std::endl;
    }
}
