#pragma once
#include "Entity.h"
#include "MirrorCube.h"

namespace gl3 {
    class PipeSwitch: public Entity {
    public:
        explicit PipeSwitch(Shader* shader, glm::vec3 position = glm::vec3(0,0,0), glm::vec3 scale = glm::vec3(0,0,0), b2WorldId physicsWorld = b2_nullWorldId);
        void onSensorEnter(Entity *other) override;
        void onSensorExit(Entity *other) override;
        bool getCanBeFlipped() {return canBeFlipped;}
        void flipSwitch();
        void setTargetCube(MirrorCube* cube) {targetCube = cube;};
        MirrorCube* getTargetCube() {return targetCube;};
        void rotatePipe (float angleInDegrees);
        void setTargetPipeSprite (Sprite* targetSprite) { pipeSprite = targetSprite; }
    protected:
        void createPhysicsBody() override;


    private:
        bool isOn = false;
        bool canBeFlipped = false;
        MirrorCube* targetCube = nullptr;
        Sprite* pipeSprite;
    };
    };





