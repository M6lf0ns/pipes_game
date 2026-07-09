#pragma once
#include "Entity.h"
#include "Vent.h"
#include "box2d/src/physics_world.h"


namespace gl3 {

    class Dog: public Entity{
        public:
        explicit Dog(Shader *shader, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 scale = glm::vec3(1, 1, 1), int floorID = 0, b2WorldId physicsWorld = b2_nullWorldId );
        void update(Game *game, float deltaTime) override;
        void draw(Game *game) override;
        void startContact(Entity* other) override;
        static float getDogRadius() {return dogRadius;};
        static void setDogRadius(float radius) {dogRadius = radius;};
        void setStickHasLanded(bool isLanded) {this->stickHasLanded = isLanded;};
        void setTargetLocation(glm::vec3 targetLoc) { this->targetLocation = targetLoc;};
        Vent* getEntryVent() {return enVent;}
        void setEntryVent(Vent* entryVent) {enVent = entryVent;};
        void setExitVent(Vent* exitVent) {exVent = exitVent;};
        Vent* getExitVent() {return exVent;}
        void setFloorID(int floorId) {this->floorID = floorId;};
        void setLocalStickFloorID(int stickID) {targetFloorID = stickID;};
        int getFloorID() const override {return floorID;};


    protected:
        void createPhysicsBody() override;

    private:
        int floorID;
        bool isWalking = false;
        GLuint idleTexture;
        int animFrame = 0;          // Which frame index are we currently showing?
        float animTimer = 0.0f;     // Tracks time passed *within* the current frame
        const float frameDuration = 0.1f; // How long each frame stays on screen (0.1s = 10 FPS)
        std::vector<GLuint> walkingFrames;
        glm::vec3 targetLocation = glm::vec3(0.0f);
        int targetFloorID = 0;
        static float dogRadius;
        bool stickHasLanded = false;
        Vent* enVent = nullptr;
        Vent* exVent = nullptr;
        bool isVisible = true;
};

};


