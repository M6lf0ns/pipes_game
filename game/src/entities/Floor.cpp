#include "Floor.h"

#include <iostream>
#include "../Game.h"
#include "box2d/box2d.h"
#include "../rendering/Texture.h"
#include "../PhysicsCategories.h"

namespace gl3 {

    Floor::Floor(Shader* shader, glm::vec3 position, glm::vec3 scale, glm::vec4 color, b2WorldId physicsWorld)
       : Entity(shader,
                position,
                scale,
                color,
                physicsWorld,
                EntityType::Floor)

    {
        this->sprite = new Sprite(position,scale, Texture::LoadTexture("assets/textures/simple_square.png"));


        Floor::createPhysicsBody();
        // Align sprite center with physics body
        sprite->setPosition(glm::vec3(position.x, floorY, 0.0f));
        sprite->setScale(glm::vec3(halfWidth * 2, floorHalfHeight * 2, 1.0f)); // full width & height


        std::cout << "Created Floor at centerY=" << floorY << std::endl;

        std::cout << "Created Floor" << std::endl;
    }

    void Floor::createPhysicsBody()
    {
        std::cout << "Box: physicsWorld.index1 = " << physicsWorld.index1 << std::endl;

        if (physicsWorld.index1 < 1 || physicsWorld.index1 > 32) {
            std::cout << "ERROR: Box has invalid physicsWorld!" << std::endl;
            throw std::runtime_error("Invalid physics world in Box");
        }
        // Box2D expects position = center of the body
        // Compute half-height
        floorHalfHeight = scale.y;  // assume scale.y already = half-height
        halfWidth = scale.x;        // assume scale.x = half-width

        // Calculate floor center Y: bottom of screen (-1.0) + half-height
        float bottomY = -1.0f; // bottom of your orthographic screen
        floorY = bottomY + floorHalfHeight;

        // Define body
        b2BodyDef groundBodyDef = b2DefaultBodyDef();
        groundBodyDef.type = b2_staticBody; // <--- ADD THIS LINE
        groundBodyDef.position = b2Vec2{ position.x, floorY };
        groundBodyDef.userData = this;

        body = b2CreateBody(physicsWorld, &groundBodyDef);

        // Define box shape (half sizes)
        b2Polygon groundBox = b2MakeBox(halfWidth, floorHalfHeight);
        b2ShapeDef groundShapeDef = b2DefaultShapeDef();
        groundShapeDef.filter.categoryBits = gl3::PhysicsCategories::CATEGORY_FLOOR;
        groundShapeDef.filter.maskBits = 0xFFFF;  // Floor collides with EVERYTHING
        shape = b2CreatePolygonShape(body, &groundShapeDef, &groundBox);  // FIXED: store shape!
        b2Shape_SetUserData(shape, this);  // FIXED: set user data on shape
        b2Shape_EnableContactEvents(shape, true);  // Now this works!


        std::cout << "Floor physics body created: centerY=" << floorY
                  << ", halfWidth=" << halfWidth << ", halfHeight=" << floorHalfHeight << std::endl;
    }

    float Floor::GetFloorY() const {
        return floorY;
    }

    float Floor::GetHalfHeight() const {
        return floorHalfHeight;
    }

}