#pragma once
#include "Entity.h"
#include "../Sprite.h"
#include "box2d/types.h"
#include "../rendering/Texture.h"
#include "box2d/src/shape.h"

namespace gl3 {

    class Door: public Entity {
        public:
        explicit Door(Shader* shader, glm::vec3 position = glm::vec3(0,0,0), glm::vec3 scale = glm::vec3(0,0,0), b2WorldId physicsWorld = b2_nullWorldId);
        void draw(Game *game) override;
        void startContact(Entity* other) override;
        void openDoor();
        void closeDoor();
        float getHalfHeight();




        protected:
        void createPhysicsBody() override;
        void destroyPhysicsBody();

    private:
        GLuint openTex;
        GLuint closedTex;
        float halfWidth, halfHeight;
        bool isDoorOpen = false;

    };


}


