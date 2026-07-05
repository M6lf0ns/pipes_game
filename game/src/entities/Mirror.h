#pragma once
#include "Entity.h"
#include "../physics/PhysicsUtils.h"



namespace gl3 {

    class Mirror: public Entity {
    public:
        explicit Mirror(Shader* shader, glm::vec3 position = glm::vec3(0,0,0), glm::vec3 scale = glm::vec3(0,0,0), float rotation = 0.0f, b2WorldId physicsWorld = b2_nullWorldId);
        void draw(Game *game) override;
        RayCastResult castRay(b2Vec2 reflectionVec, b2Vec2 hitPoint);
        void rotateMirror(float angleInDegrees);

        protected:
        void createPhysicsBody() override;

        private:
        float rotationAngle;
        float halfWidth, halfHeight;
        float normal;
    };


}


