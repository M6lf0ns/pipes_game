#pragma once
#include "Entity.h"
#include "../Sprite.h"
#include "box2d/types.h"
#include "../rendering/Texture.h"
#include "box2d/src/shape.h"

namespace gl3 {

    class TutorialSign: public Entity {
        public:
        explicit TutorialSign(Shader* shader, glm::vec3 position = glm::vec3(0,0,0), glm::vec3 scale = glm::vec3(0,0,0), int linkID = 0, b2WorldId physicsWorld = b2_nullWorldId);
        void draw(Game *game) override;
        void onSensorEnter(Entity* other) override;
        void onSensorExit(Entity* other) override;
        void setTutorialSignTexture(GLuint texture) {tutorialSignTexture = texture;};
        void update(Game *game, float deltaTime) override;



    protected:
        void createPhysicsBody() override;

        private:
        Sprite* backgroundSprite = nullptr;
        Sprite* contentSprite = nullptr;
        float displayTimer = 0.0f;
        float displayDuration = 20.0f; // seconds before fade
        float alpha = 1.0f;
        bool isFading = false;

        int linkID;
        float halfWidth, halfHeight;
        float sensorHeight;
        bool isVisible = false;
        bool hasBeenShown = false;
        GLuint tutorialSignTexture;

};

};

