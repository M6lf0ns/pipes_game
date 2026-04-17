
#include "Switch.h"
#include "../PhysicsCategories.h"
#include <iostream>

#include "../rendering/Texture.h"
#include "box2d/box2d.h"

namespace gl3 {
    Switch::Switch(Shader* shader, glm::vec3 position, glm::vec3 scale, glm::vec4 color, b2WorldId physicsWorld)
        :Entity(shader,
            position,
            scale,
            color,
            physicsWorld,
              EntityType::Switch)

    {
        this->sprite = new Sprite(position, scale, Texture::LoadTexture("assets/textures/simple_square2.png"));
        //assuming scale.x and scale.y already halfheight and -width
        halfHeight = scale.y / 2.0f;
        halfWidth = scale.x / 2.0f;

        Switch::createPhysicsBody();
        //Align sprite center with physicsbody
        //sprite.setPosition(glm::vec3(position.x,position.y,0.0f));
        //sprite.setScale(glm::vec3(halfWidth * 2, halfHeight * 2, 1.0f)); //full width and height
    }

    void Switch::createPhysicsBody() {
        std::cout << "Switch: physicsWorld.index1 = " << physicsWorld.index1 << std::endl;

        if (physicsWorld.index1 < 1 || physicsWorld.index1 > 32) {
            std::cout << "ERROR: Switch has invalid physicsWorld!" << std::endl;
            throw std::runtime_error("Invalid physics world in Switch");
        }

        // Define body
        b2BodyDef switchBodyDef = b2DefaultBodyDef();
        switchBodyDef.type = b2_staticBody;
        switchBodyDef.position = b2Vec2{position.x, position.y};
        switchBodyDef.userData = this;
        body = b2CreateBody(physicsWorld, &switchBodyDef);

        // Define shape
        b2Polygon groundSwitch = b2MakeBox(halfWidth, halfHeight);

        b2ShapeDef groundSwitchShapeDef = b2DefaultShapeDef();
        groundSwitchShapeDef.filter.categoryBits = gl3::PhysicsCategories::CATEGORY_SWITCH;
        groundSwitchShapeDef.isSensor = true;

        groundSwitchShapeDef.userData = this;
        // ✅ CREATE shape FIRST
        shape = b2CreatePolygonShape(body, &groundSwitchShapeDef, &groundSwitch);

        // ✅ THEN set userData on it
        b2Shape_SetUserData(shape, this);
        b2Shape_EnableSensorEvents(shape, true);

        void* ud = b2Shape_GetUserData(shape);
        std::cout << "Switch shape userData = " << ud << std::endl;
    }


    float Switch::getHalfHeight() const {
        return halfHeight;
    }

    float Switch::getHalfWidth() const {
        return halfWidth;
    }

    void Switch::startContact() {
        isTouchingSomething = true;
        std::cout <<"WE ARE TOUCHING" << std::endl;

    }

    void Switch::endContact() {
        isTouchingSomething = false;

    }



}
