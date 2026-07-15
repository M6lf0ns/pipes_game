#pragma once
#include "box2d/box2d.h"
#include <glm/glm.hpp>

namespace gl3 {
    // Forward declare Entity so the header doesn't get bloated
    class Entity;
    /// @struct RayCastResult
    /// @brief Holds the data returned by raycast operations.
    ///
    /// This structure encapsulates all necessary information about a ray-object
    /// intersection, used by the LaserGuy and Mirror/MirrorCube systems.

    struct RayCastResult {
        b2ShapeId shapeId = {0}; ///< ID of the shape that was hit.
        b2Vec2 hitPoint = {0.0f, 0.0f}; ///< World coordinates of the impact.
        b2Vec2 normal = {0.0f, 0.0f};   ///< Surface normal at the impact point.
        float distanceAlongRay = 1.0f;  // 1.0 = no hit ///< Normalized distance (0 to 1) of the hit.
        Entity* hitEntity = nullptr;///< Pointer to the hit entity.
        bool didHit = false;            // Helpful flag for quick if-checks
        b2Vec2 endPoint = {0.0f, 0.0f}; ///< The final point of the ray (hit or max range).
    };
    /// @class PhysicsUtils
    /// @brief Provides static utility functions for common physics tasks.

    class PhysicsUtils {
    public:
        /// @brief Performs a raycast across the physics world.
        /// @param world The Box2D world context.
        /// @param origin Starting point of the ray.
        /// @param end The target end point of the ray.
        /// @param maskBits Collision mask for filtering.
        /// @param categoryBits Collision category for filtering.
        /// @return A RayCastResult struct populated with hit data.
        // The core method any object can call
        static RayCastResult CastRay(b2WorldId world, b2Vec2 origin, b2Vec2 end, uint64_t maskBits, uint64_t categoryBits);
    };
}