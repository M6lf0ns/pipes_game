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

        float maxSpeed = 0.8f;

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
            //b2Body_SetLinearVelocity(body, vel);  // ← ADD THIS LINE!

        }
        // Flip sprite horizontally based on movement direction
        if (vel.x > 0.0f) { //velocity positive --> sprite faces right
            sprite.setScale(glm::vec3(abs(sprite.getScale().x), sprite.getScale().y, sprite.getScale().z));
        } else if (vel.x < 0.0f) { //velocity negative --> sprite faces left
            sprite.setScale(glm::vec3(-abs(sprite.getScale().x), sprite.getScale().y, sprite.getScale().z));
        }

        bool canJump = (abs(vel.y) < 0.1f); // effectively zero
        bool spacePressed = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
        bool spaceJustPressed = spacePressed && !spaceWasPressed;
        spaceWasPressed = spacePressed;

        if (spaceJustPressed && canJump) {
            b2Body_ApplyLinearImpulseToCenter(body, {0.0f, 3.0f}, true);
        }

        // //Dashing
        // float timeElapsed = 0.0f;
        // bool dashPressed = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS;
        // if ((glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) && dashPressed ) {
        //     vel.x -= 0.2f;
        //    timeElapsed += deltaTime;
        //     if (timeElapsed > 0.02f) {
        //         vel.x = -translationSpeed;
        //
        //     }
        //
        // }



        // E key for pushing
        push_PullKeyPressed = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;

        playerVelX = vel.x;



       // updateBasedOnPhysics();

        playerPos = position;


        // Prevent any rotation
        b2Body_SetAngularVelocity(body, 0);

    }



    void Ship::draw(Game *game) {

        Entity::draw(game);


        sprite.draw(shader,game);
        // std::cout << "SHIP - World Pos: " << position.x << ", " << position.y
        //   << " Screen Pos: " << sprite.getPosition().x << ", " << sprite.getPosition().y << std::endl;



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


        b2ShapeDef shapeDef = b2DefaultShapeDef();

        // Use b2MakeBox with the correct half-extents
        float halfWidth = 0.125f;  // 0.25f / 2
        float halfHeight = 0.125f; // 0.25f / 2

        b2Polygon box = b2MakeBox(halfWidth, halfHeight);
        shape = b2CreatePolygonShape(body, &shapeDef, &box);
        b2Shape_EnableContactEvents(shape, true);
        b2Shape_EnableSensorEvents(shape, true);
        b2Shape_SetUserData(shape, this);
        b2Shape_SetFriction(shape, 0.5f);
        b2Shape_SetRestitution(shape, 0.0f); // optional: no bounce

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
        massData.mass = 1.0f;
        massData.center = b2Vec2{0.0f, 0.0f};
        massData.rotationalInertia = 0.0f; // must be explicitly set
        b2Body_SetMassData(body, massData);
        b2Body_SetLinearDamping(body, 2.0f); // tweak this value



    }

    void Ship::updateBasedOnPhysics()
    {
        Entity::updateBasedOnPhysics();
        //setting the sprite´s position, can be avoided if we add Sprite to Entity
        //would not need to be overriden anymore
        // static int counter = 0;
        // std::cout << "Ship updateBasedOnPhysics call #" << ++counter << std::endl;
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
