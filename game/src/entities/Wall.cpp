#include "Wall.h"
#include <iostream>
#include "../rendering/Texture.h"
#include "../PhysicsCategories.h"

namespace gl3 {

        Wall::Wall(Shader* shader, glm::vec3 position, glm::vec3 scale, b2WorldId physicsWorld)
            : Entity(shader,
                position,
                scale,
                physicsWorld,
                EntityType::Wall)
    {
            this->sprite = new Sprite(position, scale, Texture::LoadTexture("assets/textures/wall.png"));
            this->setZOrder(3); //FOREGROUND
            Wall::createPhysicsBody();

    }


        void Wall::createPhysicsBody() {
            b2BodyDef wallDef = b2DefaultBodyDef();
            wallDef.position = {position.x, position.y};
            wallDef.type = b2_staticBody;
            body = b2CreateBody(physicsWorld, &wallDef);

            b2Polygon boxGeometry = b2MakeBox(scale.x / 2.0f, scale.y / 2.0f);

            b2ShapeDef wallShapeDef = b2DefaultShapeDef();
            // The Wall needs to collide with everything physical
            wallShapeDef.filter.categoryBits = gl3::PhysicsCategories::CATEGORY_WALL;
            wallShapeDef.filter.maskBits = gl3::PhysicsCategories::MASK_ALL_PHYSICAL |
                gl3::PhysicsCategories::CATEGORY_RAY_SENSOR;

            wallShapeDef.enableContactEvents = true;
            shape = b2CreatePolygonShape(body, &wallShapeDef, &boxGeometry);
            b2Shape_SetUserData(shape, this);
        }
    };


