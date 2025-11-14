#include <iostream>
#include "Planet.h"
#include "box2d/box2d.h"

namespace gl3 {
    struct glData {
        std::vector<float> vertices;
        std::vector<unsigned int> indices;
    };

    glData getCircleVertices(float radius) {
        glData circleData;
        float x = 0.0f, y = 0.0f, z = 0.0f;
        int n = 37;
        float angle = 90.0f;
        float step = 360.0f / (n - 1);

        int i = 0;
        while(i < n) {
            circleData.vertices.push_back(x);
            circleData.vertices.push_back(y);
            circleData.vertices.push_back(z);

            x = cos(glm::radians(angle - step * (float) i)) * radius;
            y = sin(glm::radians(angle - step * (float) i)) * radius;

            if(i + 1 < n) {
                circleData.indices.push_back(0);
                circleData.indices.push_back(i + 1);
                circleData.indices.push_back(i + 2);
            }
            i++;
        }

        circleData.indices.push_back(0);
        circleData.indices.push_back(n - 1);
        circleData.indices.push_back(1);

        return circleData;
    }

    const glData glCircleData = getCircleVertices(0.075f);

    Planet::Planet(glm::vec3 position, float size, glm::vec4 color, b2WorldId physicsWorld)
        : Entity(Shader("shaders/vertexShader.vert", "shaders/fragmentShader.frag"),
                 position,
                 0,
                 glm::vec3(size, size, size),
                 color,
                 physicsWorld)
    {
        Planet::createPhysicsBody();
    }

    void Planet::createPhysicsBody()
    {

        b2BodyDef bodyDef = b2DefaultBodyDef();
        bodyDef.type = b2_staticBody;
        bodyDef.position = {position.x, position.y};
        bodyDef.userData = this;
        body = b2CreateBody(physicsWorld, &bodyDef);

        b2ShapeDef shapeDef = b2DefaultShapeDef();
        shapeDef.density = 1.0f;

        b2Circle circle;
        // local center
        circle.center = b2Vec2{0.f, 0.0f};
        // 1 in glm equals 0.0025 in box2d, be aware that the scale influences the mass of the body
        circle.radius = scale.x * 0.0025f;

        shape = b2CreateCircleShape(body, &shapeDef, &circle);
        b2Shape_SetFriction(shape, 0.5f);
        b2Shape_SetRestitution(shape, 0.1f);
    }

    void Planet::startContact() {
        std::cout << "Planet start contact" << std::endl;
    }
}
