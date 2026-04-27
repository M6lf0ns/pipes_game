#pragma once
#include "box2d/box2d.h"
#include "../entities/Entity.h"


namespace gl3 {

    class ContactListener {
    public:
        static void checkForCollision(b2WorldId world) {
            b2ContactEvents contactEvents = b2World_GetContactEvents(world);

            for (int i = 0; i < contactEvents.beginCount; ++i) {
                b2ContactBeginTouchEvent* beginEvent = contactEvents.beginEvents + i;
                const auto bodyUserDataA = getUserDataOfShape(beginEvent->shapeIdA);
                const auto bodyUserDataB = getUserDataOfShape(beginEvent->shapeIdB);

                if(bodyUserDataA)
                {
                    auto entity = static_cast<Entity*>(bodyUserDataA);
                    //entity->startContact();
                }

                if(bodyUserDataB)
                {
                    auto entity = static_cast<Entity*>(bodyUserDataB);
                    //entity->startContact();
                }
            }
        }

    private:
        static void* getUserDataOfShape(const b2ShapeId& shape) {
            return b2Body_GetUserData(b2Shape_GetBody(shape));
        }
    };
}
