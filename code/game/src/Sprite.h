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
        Sprite(glm::vec3 pos, glm::vec3 scale, GLuint tex );

        /// @brief Draws the sprite using the provided shader and MVP matrix.
        /// @param shader Reference to the active shader program.
        /// @param mvp The transformation matrix (Model-View-Projection).
        void draw(Shader& shader, const glm::mat4& mvp);
        /// @brief Sets the rotation of the sprite.
        /// @param degrees Rotation angle in degrees.
        void setRotation(float degrees) { rotation = degrees; }

        void setPosition(glm::vec3 pos) { position = pos; }

        void setScale(glm::vec3 size) { scale = size; }

        void setColor(glm::vec4 newColor) { color = newColor; }

        glm::vec3 getPosition() {return position; }

        glm::vec3 getScale() {return scale; } ;
        glm::vec4 getColor() { return color; }
        /// @brief Updates the sprite texture.
        /// @param newTexture New OpenGL texture ID.
        void setTexture(GLuint newTexture);



        void cleanup();

    private:

        glm::vec3 position;
        glm::vec3 scale;
        float rotation; // optional if you want rotation
        glm::vec4 color = glm::vec4(1.0f); // Defaults to white (no tint)
        GLuint textureID;
        static GLuint VAO, VBO;
        static bool initialized;

        static void initRenderData();

    };



}


