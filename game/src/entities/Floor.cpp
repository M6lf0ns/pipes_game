#include "Floor.h"

#include <iostream>
#include "../Game.h"
#include "box2d/box2d.h"
#include "../rendering/Texture.h"

namespace gl3 {

    Floor::Floor(glm::vec3 position, glm::vec3 scale, glm::vec4 color, b2WorldId physicsWorld)
       : Entity(Shader("shaders/vertexShader.vert", "shaders/fragmentShader.frag"),
                position,
                0,
                scale,
                color,
                physicsWorld),
                sprite(position,scale, Texture::LoadTexture("assets/textures/simple_square.png"))

    {

        Floor::createPhysicsBody();
        // Align sprite center with physics body
        sprite.setPosition(glm::vec3(position.x, floorY, 0.0f));
        sprite.setScale(glm::vec3(halfWidth * 2, floorHalfHeight * 2, 1.0f)); // full width & height


        std::cout << "Created Floor at centerY=" << floorY << std::endl;

        std::cout << "Created Floor" << std::endl;
    }

    void Floor::createPhysicsBody()
    {
        // Box2D expects position = center of the body
        // Compute half-height
        floorHalfHeight = scale.y;  // assume scale.y already = half-height
        halfWidth = scale.x;        // assume scale.x = half-width

        // Calculate floor center Y: bottom of screen (-1.0) + half-height
        float bottomY = -1.0f; // bottom of your orthographic screen
        floorY = bottomY + floorHalfHeight;

        // Define body
        b2BodyDef groundBodyDef = b2DefaultBodyDef();
        groundBodyDef.position = b2Vec2{ position.x, floorY }; // center of floor
        b2BodyId groundId = b2CreateBody(physicsWorld, &groundBodyDef);

        // Define box shape (half sizes)
        b2Polygon groundBox = b2MakeBox(halfWidth, floorHalfHeight);
        b2ShapeDef groundShapeDef = b2DefaultShapeDef();
        b2CreatePolygonShape(groundId, &groundShapeDef, &groundBox);

        std::cout << "Floor physics body created: centerY=" << floorY
                  << ", halfWidth=" << halfWidth << ", halfHeight=" << floorHalfHeight << std::endl;

    }

    void Floor::draw(Game *game) {
        sprite.draw(shader);
    }

    float Floor::GetFloorY() const {
        return floorY;
    }

    float Floor::GetHalfHeight() const {
        return floorHalfHeight;
    }

    void Floor::startContact() {
        std::cout << "Floor::startContact" << std::endl;
    }

}