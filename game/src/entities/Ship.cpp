#include "Ship.h"

#include <iostream>

#include "../Game.h"
#include "../Assets.h"


namespace gl3 {
    Ship::Ship(glm::vec3 position, float zRotation, glm::vec3 scale, b2WorldId physicsWorld) : Entity(
            Shader("shaders/vertexShader.vert", "shaders/fragmentShader.frag"),
            Mesh({
                // positions (x, y, z)
-0.5f, -0.5f, 0.0f,
0.5f, -0.5f, 0.0f,
0.5f,  0.5f, 0.0f,
-0.5f,  0.5f, 0.0f
                 },
                 {0, 1, 2,
                  2,3, 0}),
            position,
            zRotation,
            scale,
            {1, 1, 1, 1},
            physicsWorld)
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

        if(glfwGetKey(window, GLFW_KEY_A) == GLFW_RELEASE || glfwGetKey(window, GLFW_KEY_D) == GLFW_RELEASE) {
            b2Body_SetAngularVelocity(body, 0);
        }

        if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            b2Body_ApplyTorque(body, rotationSpeed, true);
        }

        if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            b2Body_ApplyTorque(body, -rotationSpeed, true);
        }

        if(glfwGetKey(window, GLFW_KEY_W) == GLFW_RELEASE || glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE) {
            b2Body_SetLinearVelocity(body, b2Vec2_zero);
        }

        if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            float x = cos(glm::radians(zRotation)) * translationSpeed;
            float y = sin(glm::radians(zRotation)) * translationSpeed;
            b2Body_ApplyLinearImpulseToCenter(body, b2Vec2{x, y}, true);
        }

        if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            float x = -cos(glm::radians(zRotation)) * translationSpeed;
            float y = -sin(glm::radians(zRotation)) * translationSpeed;
            b2Body_ApplyLinearImpulseToCenter(body, b2Vec2{x, y}, true);
        }

        countdownUntilNextShot -= deltaTime;
        if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && countdownUntilNextShot <= 0) {
            audio.play(firingSound);
            auto angle = glm::radians(zRotation);
            glm::vec3 forwardVec = {glm::cos(angle), glm::sin(angle), 0.f};
            glm::vec3 offset {forwardVec.x * getScale().x / 2 , forwardVec.y * getScale().y / 2, 0};
            // - 90 because mesh up is +y
            auto missile =
                std::make_unique<Missile>(game, game->getShip()->position + offset, zRotation - 90, 0.05f);
            missiles.push_back(std::move(missile));
            countdownUntilNextShot = timeBetweenShots;
        }
        for (auto &m: missiles) {
            m->update(game, deltaTime);
        }
        if (missiles.size() >= 100) {
            missiles.erase(missiles.begin());
        }
    }

    void Ship::draw(Game *game) {
        Entity::draw(game);
        for (auto &m: missiles) {
            m->draw(game);
        }
    }

    void Ship::createPhysicsBody()
    {
        b2BodyDef bodyDef = b2DefaultBodyDef();
        bodyDef.type = b2_dynamicBody;
        bodyDef.position = {position.x, position.y};
        bodyDef.rotation = b2MakeRot(glm::radians(zRotation));
        bodyDef.userData = this;
        body = b2CreateBody(physicsWorld, &bodyDef);

        b2ShapeDef shapeDef = b2DefaultShapeDef();


        b2Vec2 vertices[] =
        {
            {0.5f, 0.025f},
            {0.0f, 0.3f},
            {-0.2f, 0.05f},
            {0.5f, -0.025f},
            {0.0f, -0.3f},
            {-0.2f, -0.05f}
        };

        for (auto& point: vertices)
        {
            point.x *= scale.x;
            point.y *= scale.y;
        }

        b2Hull hull = b2ComputeHull(vertices, 6);
        b2Polygon polygon = b2MakePolygon(&hull, 0.1f);
        shape = b2CreatePolygonShape(body, &shapeDef, &polygon);
        b2Shape_SetFriction(shape, 0.5f);
        b2Shape_SetRestitution(shape, 0.1f);

        b2MassData massData;
        massData.mass = 1.0f;
        massData.center = b2Vec2{0.0f, 0.0f};
        massData.rotationalInertia = 1.f;
        b2Body_SetMassData(body, massData);
    }

    void Ship::updateBasedOnPhysics()
    {
        Entity::updateBasedOnPhysics();

        // Don't forget to update the missiles
        for (auto& m: missiles)
        {
            m->updateBasedOnPhysics();
        }
    }


    void Ship::startContact() {
        std::cout << "Ship start contact" << std::endl;
    }
}
