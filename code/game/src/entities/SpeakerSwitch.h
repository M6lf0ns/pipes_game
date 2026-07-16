#pragma once
#include "Entity.h"
#include "Speaker.h"
#include "box2d/src/physics_world.h"

namespace gl3 {
    class SpeakerSwitch: public Entity {
        public:
        explicit SpeakerSwitch(Shader* shader, glm::vec3 position = glm::vec3(0,0,0), glm::vec3 scale = glm::vec3(0,0,0), b2WorldId physicsWorld = b2_nullWorldId);
        void onSensorEnter(Entity *other) override;
        void onSensorExit(Entity *other) override;
        bool getCanBeFlipped() {return canBeFlipped;}
        void update(Game *game, float deltaTime) override;
        void flipSwitch();
        void setTargetSpeaker(Speaker* speaker) {targetSpeaker = speaker;};
    protected:
        void createPhysicsBody() override;


    private:
        bool isOn = false;
        bool wasOn = false;
        bool canBeFlipped = false;
        Speaker* targetSpeaker = nullptr;
    };

}


