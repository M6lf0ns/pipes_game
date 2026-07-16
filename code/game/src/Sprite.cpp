#include <iostream>
#include "Sprite.h"
#include "Game.h"


namespace gl3 {


     GLuint Sprite::VAO = 0; // config object that remembers how to interpret VBO data
     GLuint Sprite::VBO = 0; // memory buffer on gpu that stores vertex data
     bool Sprite::initialized = false;

     Sprite::Sprite(glm::vec3 pos, glm::vec3 scale, GLuint tex)
         : position(pos), scale(scale), textureID(tex)
     {
         if (!initialized) {
             initRenderData();
             initialized = true;
         }
     }

     void Sprite::initRenderData() { //Quad that will be used by every entity to render the image onto it
         if (VAO != 0) {
             return;
         }
         float vertices[] = {
             // positions   // tex coords
             0.5f,  0.5f,   1.0f, 1.0f, // Top-Right -> 1.0, 1.0
             -0.5f, 0.5f,   0.0f, 1.0f, // Top-Left  -> 0.0, 1.0
             -0.5f, -0.5f,  0.0f, 0.0f, // Bottom-Left -> 0.0, 0.0
              0.5f, -0.5f,  1.0f, 0.0f  // Bottom-Right -> 1.0, 0.0
         };

         glGenVertexArrays(1, &VAO);
         glGenBuffers(1, &VBO);

         glBindVertexArray(VAO);
         glBindBuffer(GL_ARRAY_BUFFER, VBO);
         glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

         // position
         glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
         glEnableVertexAttribArray(0);
         // tex coords
         glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
         glEnableVertexAttribArray(1);

         glBindBuffer(GL_ARRAY_BUFFER, 0);
         glBindVertexArray(0);
     }

    // Change the signature to take the pre-calculated matrix
    void Sprite::draw(Shader& shader, const glm::mat4& mvp) {
         shader.use();


         shader.setMatrix("mvp", mvp);
         shader.setVector("spriteColor", this->color);

         glActiveTexture(GL_TEXTURE0);
         glBindTexture(GL_TEXTURE_2D, textureID);

         glDepthMask(GL_FALSE);
         glBindVertexArray(VAO);
         glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
         glBindVertexArray(0);
         glDepthMask(GL_TRUE);
     }

    void Sprite::setTexture(GLuint newTexture) {
         textureID = newTexture;
     }

     void Sprite::cleanup() {
         glDeleteVertexArrays(1, &VAO);
         glDeleteBuffers(1, &VBO);
     }

 }
