#include <iostream>
#include "box2d/box2d.h"
#include "Entity.h"
#include "../Game.h"
#include "Missile.h"

namespace gl3 {
    Missile::Missile(gl3::Game *game, glm::vec3 position, float zRotation, float size) : Entity(
            Shader("shaders/vertexShader.vert", "shaders/fragmentShader.frag"),
            position,
            zRotation,
            glm::vec3(size),
            {1.0f, 1.0f, 1.0f, 1.0f},
            game->getPhysicsWorld()
    )
    {
        Missile::createPhysicsBody();
    }

    void Missile::createPhysicsBody() {
        b2BodyDef bodyDef = b2DefaultBodyDef();
        bodyDef.type = b2_dynamicBody;
        bodyDef.position = {position.x, position.y};
        bodyDef.rotation = b2MakeRot(glm::radians(zRotation));
        //bodyDef.fixedRotation = true;

        bodyDef.isBullet = true;

        // + 90 because mesh up is +y
        b2Vec2 forwardVec = {glm::cos(glm::radians(zRotation + 90)), glm::sin(glm::radians(zRotation + 90))};
        bodyDef.linearVelocity = forwardVec * speed;

        bodyDef.userData = this;
        body = b2CreateBody(physicsWorld, &bodyDef);

        b2ShapeDef shapeDef = b2DefaultShapeDef();
        shapeDef.density = 1.0f;

        // approximation of polygon shape
        b2Vec2 vertices[] =
        {
            {0.0f, 0.5f},
            {0.2f, 0.4f},
            {0.3f, -0.4f},
            {-0.3, -0.4},
            {-0.2f, 0.4f},
        };

        for (auto &point : vertices) {
            point.x *= scale.x;
            point.y *= scale.y;
        }

        b2Hull hull = b2ComputeHull(vertices, 5);
        b2Polygon polygon = b2MakePolygon(&hull, 0.1f);
        shape = b2CreatePolygonShape(body, &shapeDef, &polygon);
        b2Shape_SetFriction(shape, 0.0f);
        b2Shape_SetRestitution(shape, 0.1f);
    }

    void Missile::startContact() {
        std::cout << "Ship start contact" << std::endl;
    }
}
