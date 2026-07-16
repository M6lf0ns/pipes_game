#pragma once
#include "Entity.h"

#include "../Sprite.h"
#include "box2d/types.h"
#include "../rendering/Texture.h"
#include "box2d/src/shape.h"

namespace gl3 {

    class Ladder: public Entity {
    public:
        explicit Ladder(Shader* shader, glm::vec3 position = glm::vec3(0,0,0), glm::vec3 scale = glm::vec3(0,0,0), b2WorldId physicsWorld = b2_nullWorldId);
        void draw(Game *game) override;
        void onSensorEnter(Entity* other) override;
        void onSensorExit(Entity* other) override;
        // Change this in Ladder.h
        float getTopY() { return this->position.y + this->halfHeight; }




    protected:
        void createPhysicsBody() override;
        void destroyPhysicsBody();

    private:
        float halfWidth, halfHeight;
        float sensorHeight;


    };


}