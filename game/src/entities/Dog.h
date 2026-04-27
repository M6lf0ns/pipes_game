#pragma once
#include "Entity.h"
#include "box2d/src/physics_world.h"


namespace gl3 {

    class Dog: public Entity{
        public:
        explicit Dog(Shader *shader, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 scale = glm::vec3(1, 1, 1), b2WorldId physicsWorld = b2_nullWorldId );
        void update(Game *game, float deltaTime) override;
        void startContact(Entity* other) override;
        static float getDogRadius() {return dogRadius;};
        static void setDogRadius(float radius) {dogRadius = radius;};
        void setStickHasLanded(bool isLanded) {this->stickHasLanded = isLanded;};
        void setTargetLocation(float targetLoc) { this->targetLocation = targetLoc;};

    protected:
        void createPhysicsBody() override;

    private:
        float targetLocation = 0.0f;
        static float dogRadius;
        bool stickHasLanded = false;
};

};


