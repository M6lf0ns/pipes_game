#include "Mirror.h"
#include "../EntityType.h"
#include "../rendering/Texture.h"
#include "box2d/box2d.h"
#include "../PhysicsCategories.h"
#include <iostream>
#include "../physics/PhysicsUtils.h"

namespace gl3 {
    Mirror::Mirror(Shader* shader, glm::vec3 position, glm::vec3 scale, float rotation, b2WorldId physicsWorld)
      :Entity(shader,
          position,
          scale,
          physicsWorld,
          EntityType::Mirror),
    rotationAngle(rotation)

    {
        this->sprite = new Sprite(position, scale, Texture::LoadTexture("assets/textures/mirror.png"));
        this->setZOrder(2);//MIDGROUND
        halfWidth = scale.x / 2.0f;
        halfHeight = scale.y / 2.0f;

        Mirror::createPhysicsBody();
        this->rotateMirror(rotationAngle);

        //Align sprite center with physicsbody
        sprite->setPosition(glm::vec3(position.x,position.y,0.0f));
        sprite->setScale(glm::vec3(halfWidth * 2, halfHeight * 2, 1.0f)); //full width and height
    }


    void Mirror::createPhysicsBody() {

        b2BodyDef mirrorBodyDef = b2DefaultBodyDef();
        mirrorBodyDef.type = b2_staticBody;
        mirrorBodyDef.position = b2Vec2{position.x,position.y};
        mirrorBodyDef.userData = this;
        body = b2CreateBody(physicsWorld, &mirrorBodyDef);

        b2Polygon mirrorShape = b2MakeBox(halfWidth/ 2.0f, halfHeight);
        b2ShapeDef mirrorShapeDef = b2DefaultShapeDef();
        mirrorShapeDef.filter.categoryBits = gl3::PhysicsCategories::CATEGORY_MIRROR;
        mirrorShapeDef.filter.maskBits = gl3::PhysicsCategories::CATEGORY_RAY_SENSOR;
        mirrorShapeDef.enableSensorEvents = true;
        shape = b2CreatePolygonShape(body, &mirrorShapeDef, &mirrorShape);
        b2Shape_SetUserData(shape, this);

    }

    void Mirror::draw(Game *game) {
        Entity::draw(game);
    }


    RayCastResult Mirror::castRay(b2Vec2 reflectionVec, b2Vec2 hitPoint) {
        b2Vec2 origin = hitPoint + (0.05f * reflectionVec); // Step out slightly to avoid re-hitting
        b2Vec2 end = origin + (reflectionVec * 20.0f);

        uint64_t category = gl3::PhysicsCategories::CATEGORY_RAY_SENSOR;
        uint64_t mask = gl3::PhysicsCategories::CATEGORY_WALL | gl3::PhysicsCategories::CATEGORY_BOX |
            gl3::PhysicsCategories::CATEGORY_LASER_EYES;

        // Run the reflection raycast and return the whole result struct!
        return PhysicsUtils::CastRay(physicsWorld, origin, end, mask, category);

    }
    void Mirror::rotateMirror(float angleInDegrees) {
        // 1. ABSOLUTE SETTING: No +=, just =
        this->rotationAngle = angleInDegrees;

        // 2. CONVERSION: CCW Radians
        float radians = glm::radians(angleInDegrees);

        // 3. TRANSFORM: Force absolute orientation
        // Using b2Body_SetTransform resets the rotation entirely
        b2Vec2 bodyPos = b2Body_GetPosition(body);
        b2Body_SetTransform(body, bodyPos, b2MakeRot(radians));

        // 4. VISUALS
        sprite->setRotation(radians);
        std::cout << "ROTATED PIPE TO ANGLE: " <<  angleInDegrees << std::endl;
    }

}