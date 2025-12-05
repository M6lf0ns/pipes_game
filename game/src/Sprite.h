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

        void draw(Shader& shader, Game* game);
        void setPosition(glm::vec3 pos) { position = pos; }
        void setScale(glm::vec3 size) { scale = size; }
        glm::vec3 getPosition() {return position; }
        glm::vec3 getScale() {return scale; } ;
        void setTexture(GLuint newTexture);



        void cleanup();

    private:

        glm::vec3 position;
        glm::vec3 scale;
        float rotation; // optional if you want rotation

        GLuint textureID;
        static GLuint VAO, VBO;
        static bool initialized;

        static void initRenderData();

    };



}


