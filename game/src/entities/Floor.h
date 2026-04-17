#pragma once
#include "Entity.h"
#include "../Sprite.h"

namespace gl3 {

    class Floor: public Entity {
    public:
        explicit Floor(Shader* shader, glm::vec3 position = glm::vec3(0, 0, 0), glm::vec3 scale = glm::vec3(0.0f,0.0f,0.0f), glm::vec4 color = glm::vec4(1, 0, 0, 1), b2WorldId physicsWorld = b2_nullWorldId);
        [[nodiscard]] float GetHalfHeight() const;
        [[nodiscard]] float GetFloorY() const;



    protected:
        void createPhysicsBody() override;



    private:

        float halfWidth = 0.0f;
        float floorHalfHeight = 0.0f;
        float floorY = 0.0f;
    };


}


