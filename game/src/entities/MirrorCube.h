#pragma once
#include "Entity.h"
#include "../physics/PhysicsUtils.h"

namespace gl3 {

    class MirrorCube: public Entity {

    public:
        explicit MirrorCube(Shader* shader, glm::vec3 position = glm::vec3(0,0,0), glm::vec3 scale = glm::vec3(0,0,0), float rotationAngle = 0.0f, b2WorldId physicsWorld = b2_nullWorldId);
        void draw(Game *game) override;
        void update(Game *game, float deltaTime) override;
        RayCastResult castRay(b2Vec2 reflectionVec, b2Vec2 hitPoint);
        void rotatePipe (float angleInDegrees);
        float getPipeRotation() { return this->rotationAngle; }
        void setPipeRotation(int angle) { this->rotationAngle = angle; };
        float getPipeHalfWidth() { return this->pipeHalfWidth; }
        Sprite* getPipeSprite() {
            return this->pipeSprite;
        }



    protected:
        void createPhysicsBody() override;

    private:
        float halfWidth, halfHeight;
        float pipeHalfWidth, pipeHalfHeight;
        float normal;
        Sprite* backgroundSprite;
        Sprite* pipeSprite;
        float rotationAngle;
        MirrorCube* hitCube= nullptr;
    };
    }




