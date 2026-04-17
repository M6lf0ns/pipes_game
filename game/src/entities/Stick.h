#pragma once
#include "Dog.h"
#include "Entity.h"
#include "../Sprite.h"
#include "box2d/collision.h"

namespace gl3 {
    class Stick : public Entity {
    public:
        Stick(Shader* shader, glm::vec3 position = glm::vec3(0,0,0), glm::vec3 scale = glm::vec3(0.0f,0.0f,0.0f), b2WorldId physicsWorld = b2_nullWorldId);
        void update(Game *game, float deltaTime) override;
        void draw(Game *game) override;
        void simulateThrow(Game *game, glm::vec3 bodyPos, float direction);
        void throwStick(Game *game, glm::vec3 pos, float direction);
        void drawTrajectory(Game *game);
        void setAiming( bool isAiming);
        void setDawg(Dog* dog);
        Dog* getDawg() {return dawg;}
        void startContact() override;
        void endContact();
        ~Stick() override;




    protected:
        void createPhysicsBody() override;

    private:
        Dog* dawg = nullptr;
        bool hasLanded = false;
        bool hasBeenThrown = false;
        float impulse;
        bool isAiming = false;
        float mass;
        std::vector<glm::vec3> trajectoryPoints;
        unsigned int vao, vbo;





    };
}
