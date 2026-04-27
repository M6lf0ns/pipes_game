#pragma once
#include "../entities/Entity.h"
#include <memory>
#include <vector>

#include "box2d/id.h"
namespace gl3 {
    class Game;

    class Scene {

    public:
        enum Layers {
            BACKGROUND = -10,
            MIDGROUND = 0,
            PLAYER = 5,
            FOREGROUND = 10,
            UI = 100
        };

        Scene(Game* gamePtr) : game(gamePtr) {}
        virtual ~Scene();
        void loadLevelData(const std::string& levelName, b2WorldId world);
        virtual void load(b2WorldId physicsWorld);
        virtual void update(float dt);
        virtual void draw();
        void updatePhysics(float dt);
        void setDeltatime();
        void setPhysicsWorld(b2WorldId world) {PhysicsWorld = world;};
        void setGame(Game* gamePtr) { this->game = gamePtr; }
        int getLevelID() const { return levelID; }




    protected:
        int levelID;
        b2WorldId PhysicsWorld;
        std::unique_ptr<gl3::Shader> spriteShader;
        std::vector<std::unique_ptr<Entity>> entities;
        Game* game = nullptr;

    private:


        float accumulator = 0.0f;


        float deltatime;

    };


}



