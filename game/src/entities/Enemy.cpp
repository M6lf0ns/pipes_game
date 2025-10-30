#include <iostream>

#include "../Game.h"
#include "Enemy.h"

namespace gl3
{
    Enemy::Enemy(glm::vec3 position, float zRotation, float size, b2WorldId physicsWorld)
            : Entity(Shader("shaders/vertexShader.vert", "shaders/fragmentShader.frag"),
                     Mesh({0.0f, 0.0f, 0.0f,
                           0.0f, 0.5f, 0.0f,
                           0.25f, 0.25f, 0.0f,
                           0.5f, 0.5f, 0.0f,
                           0.75f, 0.0f, 0.0f,
                           1.0f, -0.1f, 0.0f,
                           0.1f, -0.25f, 0.0f,
                           0.0f, -0.75f, 0.0f,

                           -0.1f, -0.25f, 0.0f,
                           -1.0f, -0.1f, 0.0f,
                           -0.75f, 0.0f, 0.0f,
                           -0.5f, 0.5f, 0.0f,
                           -0.25f, 0.25f, 0.0f},
                          {0, 1, 2,
                           0, 2, 3,
                           0, 3, 4,
                           0, 4, 5,
                           0, 5, 6,
                           0, 6, 7,
                           0, 7, 8,
                           0, 8, 9,
                           0, 9, 10,
                           0, 10, 11,
                           0, 11, 12,
                           0, 12, 1}),
                     position,
                     zRotation,
                     glm::vec3(size, size, size),
                     glm::vec4(0.35f, 0.35f, 0.35f, 1.0f),
                     physicsWorld
    )
    {
        Enemy::createPhysicsBody();
    }

    float lerp(float a, float b, float f)
    {
        return a + f * (b - a);
    }

    void Enemy::update(Game* game, float deltaTime)
    {
        const auto shipPosition = game->getShip()->getPosition();
        auto distanceToShip = glm::distance(position, shipPosition);
        float delta_x = this->getPosition().x - shipPosition.x;
        float delta_y = this->getPosition().y - shipPosition.y;
        // mesh forward is +y instead of x+ so we need to add 90
        float theta_radians = atan2(delta_y, delta_x) - glm::radians(90.f);

        if (!b2Body_IsValid(body))
            return;

        b2Rot newRotation;
        newRotation.c = glm::cos(theta_radians);
        newRotation.s = glm::sin(theta_radians);
        b2Body_SetTransform(body, b2Body_GetPosition(body), newRotation);

        // -90 because mesh forward is +y
        float adjustedAngle = glm::radians(zRotation - 90);
        b2Vec2 forwardVec = {glm::cos(adjustedAngle), glm::sin(adjustedAngle)};
        if (distanceToShip >= 0.5f)
        {
            b2Body_SetLinearVelocity(body, forwardVec * lerp(minSpeed, maxSpeed, distanceToShip));
        }

        std::time_t elapsedTime = std::time(nullptr);
        countdownUntilNextShot -= deltaTime;
        if (elapsedTime % 5 == 0 && countdownUntilNextShot <= 0)
        {
            // -90 because mesh forward is +y
            auto angle = glm::radians(zRotation - 90);
            glm::vec3 forwardVec = {glm::cos(angle), glm::sin(angle), 0.f};
            glm::vec3 offset{forwardVec.x * getScale().x, forwardVec.y * getScale().y, 0};
            auto missile =
                    std::make_unique<Missile>(game, position + offset, zRotation - 180, 0.05f);
            missiles.push_back(std::move(missile));
            countdownUntilNextShot = timeBetweenShots;
        }

        for (auto& m: missiles)
        {
            m->update(game, deltaTime);
        }
        if (missiles.size() >= 100)
        {
            missiles.erase(missiles.begin());
        }
    }

    void Enemy::draw(Game* game)
    {
        Entity::draw(game);
        for (auto& m: missiles)
        {
            m->draw(game);
        }
    }

    void Enemy::createPhysicsBody()
    {
        b2BodyDef bodyDef = b2DefaultBodyDef();
        bodyDef.type = b2_dynamicBody;
        bodyDef.position = {position.x, position.y};
        bodyDef.rotation = b2MakeRot(glm::radians(zRotation));
        bodyDef.userData = this;
        body = b2CreateBody(physicsWorld, &bodyDef);

        b2ShapeDef shapeDef = b2DefaultShapeDef();

        // approximation of polygon shape
        b2Vec2 vertices[] =
        {
            {0.0f,  0.5f},
            {1.0f,  0.1f},
            {-1.0f, 0.1f},
            {0.0f,  -1.f}
        };

        for (auto& point: vertices)
        {
            point.x *= scale.x;
            point.y *= scale.y;
        }

        b2Hull hull = b2ComputeHull(vertices, 4);
        b2Polygon polygon = b2MakePolygon(&hull, 0.1f);
        shape = b2CreatePolygonShape(body, &shapeDef, &polygon);
        b2Shape_SetFriction(shape, 0.5f);
        b2Shape_SetRestitution(shape, 0.1f);
    }

    void Enemy::updateBasedOnPhysics()
    {
        Entity::updateBasedOnPhysics();

        for (const auto& m: missiles)
        {
            m->updateBasedOnPhysics();
        }
    }

    void Enemy::startContact()
    {
        std::cout << "Enemy start contact" << std::endl;
    }
}
