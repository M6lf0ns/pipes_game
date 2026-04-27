#include "Floor.h"

#include <iostream>
#include "../Game.h"
#include "box2d/box2d.h"
#include "../rendering/Texture.h"
#include "../PhysicsCategories.h"

namespace gl3 {

    // Floor.cpp
    Floor::Floor(Shader* shader, glm::vec3 position, glm::vec3 scale, b2WorldId physicsWorld, const std::string& texturePath)
       : Entity(shader, position, scale, physicsWorld, EntityType::Floor)
    {
        // 1. Set these FIRST so createPhysicsBody can use them
        this->halfWidth = scale.x / 2.0f;
        this->floorHalfHeight = scale.y / 2.0f;
        this->floorY = position.y; // Use the position passed from the level

        // Use the variable here!
        this->sprite = new Sprite(position, scale, Texture::LoadTexture(texturePath.c_str()));
        this->setZOrder(0); //BACKGROUND
        createPhysicsBody();

        // 3. Align sprite (use the consistent position)
        sprite->setPosition(glm::vec3(position.x, position.y, 0.0f));
        sprite->setScale(glm::vec3(scale.x, scale.y, 1.0f));
    }

    void Floor::createPhysicsBody() {
        b2BodyDef groundBodyDef = b2DefaultBodyDef();
        groundBodyDef.type = b2_staticBody;

        // Use the exact position passed to the constructor
        groundBodyDef.position = b2Vec2{ position.x, position.y };
        groundBodyDef.userData = this;

        body = b2CreateBody(physicsWorld, &groundBodyDef);

        // b2MakeBox takes HALF widths/heights
        b2Polygon groundBox = b2MakeBox(halfWidth, floorHalfHeight);

        b2ShapeDef groundShapeDef = b2DefaultShapeDef();
        groundShapeDef.filter.categoryBits = gl3::PhysicsCategories::CATEGORY_FLOOR;
        groundShapeDef.filter.maskBits = 0xFFFF;

        shape = b2CreatePolygonShape(body, &groundShapeDef, &groundBox);
        b2Shape_SetUserData(shape, this);
    }

    float Floor::GetFloorY() const {
        return floorY;
    }

    float Floor::GetHalfHeight() const {
        return floorHalfHeight;
    }

}