#pragma once
#include "Entity.h"

namespace gl3 {
    class Speaker: public Entity {
    public:
        explicit Speaker(Shader* shader, glm::vec3 position = glm::vec3(0,0,0), glm::vec3 scale = glm::vec3(0,0,0), b2WorldId physicsWorld = b2_nullWorldId);
        void onSensorEnter(Entity* other) override;
        void onSensorExit(Entity* other) override;
        void setCurrentTarget(Entity* target) {currentTarget = target;};
        void StartNoise();
        void CeaseNoise();
        void update(Game *game, float deltaTime) override;
        bool getIsActive() {return isActive;};


    protected:
        void createPhysicsBody() override;

    private:
        float soundTimer = 0.0f;
        const float SOUND_INTERVAL = 0.5f; // Play sound every 0.5 seconds
        bool botEnteredWhileActive = false;
        Entity* currentTarget = nullptr;
        bool isActive = false;
        std::vector<GLuint> speakerOnFrames;
        GLuint speakerOffTexture;
        int animFrame = 0;          // Which frame index are we currently showing?
        float animTimer = 0.0f;     // Tracks time passed *within* the current frame
        const float frameDuration = 0.1f; // How long each frame stays on screen (0.1s = 10 FPS)















    };
}



