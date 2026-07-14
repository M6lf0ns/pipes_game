#pragma once
#include "../entities/Entity.h"
#include <memory>
#include <vector>
#include "box2d/id.h"

namespace gl3 {
    class Game;

    /// @class Scene
    /// @brief Manages the lifecycle, rendering, and physics for a specific game level.
    ///
    /// The Scene acts as a container for all game entities and coordinates the
    /// communication between the Game engine, the physics world, and the renderer.
    class Scene {

    public:
        /// @brief Defines render layers for depth sorting.
        enum Layers {
            BACKGROUND = -10, ///< Background layer.
            MIDGROUND = 0,    ///< Default midground layer.
            PLAYER = 5,       ///< Player layer.
            FOREGROUND = 10,  ///< Foreground layer.
            UI = 100          ///< UI overlay layer.
        };

        /// @brief Constructs a new Scene associated with the game engine.
        /// @param gamePtr Pointer to the parent Game instance.
        Scene(Game* gamePtr) : game(gamePtr) {}

        /// @brief Virtual destructor to ensure proper cleanup of scene resources.
        virtual ~Scene();

        /// @brief Parses level data files and populates the physics world.
        /// @param levelName Filename of the level data.
        /// @param world The Box2D physics world.
        void loadLevelData(const std::string& levelName, b2WorldId world);

        /// @brief Initializes the scene resources and entities.
        virtual void load(b2WorldId physicsWorld);

        /// @brief Updates scene logic and entity states.
        /// @param dt Time elapsed since the last frame.
        virtual void update(float dt);

        /// @brief Draws all entities and scene elements.
        virtual void draw();

        /// @brief Advances the physics simulation.
        /// @param dt Time step for the physics simulation.
        void updatePhysics(float dt);

        /// @brief Updates the internal delta time value.
        void setDeltatime();

        /// @brief Sets the reference to the active Box2D physics world.
        void setPhysicsWorld(b2WorldId world) { PhysicsWorld = world; };

        /// @brief Updates the game engine pointer.
        void setGame(Game* gamePtr) { this->game = gamePtr; }

        /// @brief Retrieves the current level ID.
        int getLevelID() const { return levelID; }

        /// @brief Accesses the list of all entities within the scene.
        std::vector<std::unique_ptr<Entity>>& getEntities() { return entities; }

        /// @brief Static callback for Box2D pre-solve collision handling.
        /// @param shapeIdA First shape ID.
        /// @param shapeIdB Second shape ID.
        /// @param point Collision contact point.
        /// @param normal Collision contact normal.
        /// @param context User data context pointer.
        static bool MyPreSolve(b2ShapeId shapeIdA, b2ShapeId shapeIdB,
                               b2Vec2 point, b2Vec2 normal, void* context);

    protected:
        int levelID;                 ///< Unique ID of the level.
        b2WorldId PhysicsWorld;      ///< Reference to the physics simulation world.
        std::unique_ptr<gl3::Shader> spriteShader; ///< Shader used for sprite rendering.
        std::vector<std::unique_ptr<Entity>> entities; ///< Collection of all active entities.
        std::vector<gl3::Vent*> allVents; ///< Pointers to all vent entities in the scene.
        Game* game = nullptr;        ///< Pointer to the main game engine.
        Ship* ship = nullptr;        ///< Pointer to the player's ship.

    private:
        float accumulator = 0.0f;    ///< Accumulator for fixed-step physics.
        float deltatime;             ///< Current frame delta time.
    };
}