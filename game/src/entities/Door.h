#pragma once
#include "Entity.h"
#include "../Sprite.h"
#include "box2d/types.h"
#include "box2d/src/shape.h"

namespace gl3 {

    class Door: public Entity {
        public:
        explicit Door(Shader* shader, glm::vec3 position = glm::vec3(0,0,0), glm::vec3 scale = glm::vec3(0,0,0), glm::vec4 color = glm::vec4(1, 0, 0, 1), b2WorldId physicsWorld = b2_nullWorldId);
        void draw(Game *game) override;
        void startContact() override;
        void openDoor();
        void closeDoor();
        float getHalfHeight();




        protected:
        void createPhysicsBody() override;
        void destroyPhysicsBody();

    private:
        float halfWidth, halfHeight;
        bool isDoorOpen = false;

    };


}


