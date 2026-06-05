#pragma once
#include "box2d/box2d.h"
#include <glm/glm.hpp>

namespace gl3 {
    // Forward declare Entity so the header doesn't get bloated
    class Entity;

    struct RayCastResult {
        b2ShapeId shapeId = {0};
        b2Vec2 hitPoint = {0.0f, 0.0f};
        b2Vec2 normal = {0.0f, 0.0f};
        float distanceAlongRay = 1.0f;  // 1.0 = no hit
        Entity* hitEntity = nullptr;
        bool didHit = false;            // Helpful flag for quick if-checks
    };

    class PhysicsUtils {
    public:
        // The core method any object can call
        static RayCastResult CastRay(b2WorldId world, b2Vec2 origin, b2Vec2 end, uint64_t maskBits, uint64_t categoryBits);
    };
}