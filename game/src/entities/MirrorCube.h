#pragma once
#include "Entity.h"
#include "../physics/PhysicsUtils.h"


namespace gl3 {

    class MirrorCube: public Entity {

    public:
        explicit MirrorCube(Shader* shader, glm::vec3 position = glm::vec3(0,0,0), glm::vec3 scale = glm::vec3(0,0,0), float rotationAngle = 0.0f, int linkID = 0, int floorID = 0, b2WorldId physicsWorld = b2_nullWorldId);
        void draw(Game *game) override;
        void update(Game *game, float deltaTime) override;
        RayCastResult castRay(b2Vec2 reflectionVec, b2Vec2 hitPoint);
        void rotatePipe (float angleInDegrees, bool playSound);
        float getPipeRotation() { return this->rotationAngle; }
        void setPipeRotation(int angle) { this->rotationAngle = angle; };
        float getPipeHalfWidth() { return this->pipeHalfWidth; }
        void setPipeRotated (bool rotated) {pipeRotated = rotated;}
        Sprite* getPipeSprite() {
            return this->pipeSprite;
        }
        int getLinkID() { return this->linkID; }
        // ADD 'override' to match your base class contract
        int getFloorID() const override { return this->floorID; }



    protected:
        void createPhysicsBody() override;

    private:
        Game* game = nullptr;
        bool pipeRotated = false;
        bool wasPipeRotated = false;
        int floorID;
        int linkID;
        float halfWidth, halfHeight;
        float pipeHalfWidth, pipeHalfHeight;
        float normal;
        Sprite* backgroundSprite;
        Sprite* pipeSprite;
        float rotationAngle;
        MirrorCube* hitCube= nullptr;
    };
    }




