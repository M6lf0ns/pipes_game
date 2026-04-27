#pragma once
#include <glad/glad.h>
#include "glm/vec3.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "rendering/Shader.h"



namespace gl3 {
    class Game;

    class Sprite {
        public:
        Sprite(glm::vec3 pos, glm::vec3 scale, GLuint tex );

        void draw(Shader& shader, const glm::mat4& mvp);
        void setRotation(float degrees) { rotation = degrees; }
        void setPosition(glm::vec3 pos) { position = pos; }
        void setScale(glm::vec3 size) { scale = size; }
        void setColor(glm::vec4 newColor) { color = newColor; }
        glm::vec3 getPosition() {return position; }
        glm::vec3 getScale() {return scale; } ;
        glm::vec4 getColor() { return color; }
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


