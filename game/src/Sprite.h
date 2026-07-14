#pragma once
#include <glad/glad.h>
#include "glm/vec3.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "rendering/Shader.h"

namespace gl3 {
    class Game;

    /// @class Sprite
    /// @brief Handles rendering of 2D textured sprites in the game world.
    ///
    /// Manages position, scaling, rotation, and color tinting for textured quads.
    /// Uses static vertex data (VAO/VBO) for efficient rendering.
    class Sprite {
    public:
        /// @brief Constructs a new Sprite instance.
        /// @param pos Initial world position.
        /// @param scale Scaling vector for dimensions.
        /// @param tex OpenGL texture ID to be rendered.
        Sprite(glm::vec3 pos, glm::vec3 scale, GLuint tex);

        /// @brief Draws the sprite using the provided shader and MVP matrix.
        /// @param shader Reference to the active shader program.
        /// @param mvp The transformation matrix (Model-View-Projection).
        void draw(Shader& shader, const glm::mat4& mvp);

        /// @brief Sets the rotation of the sprite.
        /// @param degrees Rotation angle in degrees.
        void setRotation(float degrees) { rotation = degrees; }

        /// @brief Updates the world position of the sprite.
        /// @param pos New position vector.
        void setPosition(glm::vec3 pos) { position = pos; }

        /// @brief Updates the scale of the sprite.
        /// @param size New scaling vector.
        void setScale(glm::vec3 size) { scale = size; }

        /// @brief Applies a color tint to the sprite.
        /// @param newColor RGBA color vector.
        void setColor(glm::vec4 newColor) { color = newColor; }

        /// @brief Gets the current world position.
        /// @return Current position vector.
        glm::vec3 getPosition() { return position; }

        /// @brief Gets the current scale.
        /// @return Current scale vector.
        glm::vec3 getScale() { return scale; }

        /// @brief Gets the current color tint.
        /// @return Current RGBA color vector.
        glm::vec4 getColor() { return color; }

        /// @brief Updates the sprite texture.
        /// @param newTexture New OpenGL texture ID.
        void setTexture(GLuint newTexture);

        /// @brief Cleans up sprite-specific resources.
        void cleanup();

    private:
        glm::vec3 position;  ///< Current world position.
        glm::vec3 scale;     ///< Current dimensions/scale.
        float rotation;      ///< Current rotation in degrees.
        glm::vec4 color;     ///< Color tint (default white).
        GLuint textureID;    ///< OpenGL texture handle.

        static GLuint VAO, VBO; ///< Shared static vertex buffers.
        static bool initialized; ///< Tracking if render data is initialized.

        /// @brief Initializes shared static vertex data for all sprites.
        static void initRenderData();
    };
}