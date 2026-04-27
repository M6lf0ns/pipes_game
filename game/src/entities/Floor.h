#pragma once
#include "Entity.h"
#include "../Sprite.h"

namespace gl3 {

    class Floor: public Entity {
    public:
        explicit Floor(Shader* shader, glm::vec3 position = glm::vec3(0, 0, 0), glm::vec3 scale = glm::vec3(0.0f,0.0f,0.0f), b2WorldId physicsWorld = b2_nullWorldId, const std::string& texturePath = "assets/textures/floor1.png");
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


