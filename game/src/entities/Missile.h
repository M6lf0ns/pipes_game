#pragma once

#include "Entity.h"

namespace gl3 {
    class Missile: public Entity {
    public:
        explicit Missile(Game *game, glm::vec3 position, float zRotation, float size);

        void startContact() override;

    protected:
        void createPhysicsBody() override;

    private:
        float speed = 5.0f;
    };
}



