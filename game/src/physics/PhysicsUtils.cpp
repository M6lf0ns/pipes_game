#include "PhysicsUtils.h"
#include "../entities/Entity.h" // Path to your base Entity class

namespace gl3 {


    // Keep your exact callback logic, just marked static to protect scope
    static float MyCastCallback(b2ShapeId shapeId, b2Vec2 point, b2Vec2 normal, float fraction, void* context)
    {
        RayCastResult* myContext = (RayCastResult*)context;

        if (fraction < myContext->distanceAlongRay) {
            myContext->shapeId = shapeId;
            myContext->hitPoint = point;
            myContext->normal = normal;
            myContext->distanceAlongRay = fraction;
            myContext->didHit = true;

            void* userData = b2Shape_GetUserData(shapeId);
            if (userData) {
                myContext->hitEntity = static_cast<Entity*>(userData);
            } else {
                myContext->hitEntity = nullptr;
            }
        }
        return fraction;
    }

    RayCastResult PhysicsUtils::CastRay(b2WorldId world, b2Vec2 origin, b2Vec2 end, uint64_t maskBits, uint64_t categoryBits) {
        RayCastResult result;

        // Use the mask and category bits passed dynamically by the entity
        b2QueryFilter filter = b2DefaultQueryFilter();
        filter.categoryBits = categoryBits;
        filter.maskBits = maskBits;

        // Your exact math fix for Box2D v3's delta/translation requirement
        b2Vec2 translation = b2Sub(end, origin);

        // Fire it off!
        b2World_CastRay(world, origin, translation, filter, MyCastCallback, &result);

        return result;
    }
}