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
         float vertices[] = {
             // positions   // tex coords
             0.5f, 0.5f, 0.0f, 1.0f,
              -0.5f, 0.5f, 1.0f, 1.0f,
              -0.5f,  -0.5f, 1.0f, 0.0f,
             0.5f,  -0.5f, 0.0f, 0.0f
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

     void Sprite::draw(Shader& shader, Game* game, float rot) {
         // Sprite calculates its own MVP using game's camera
         glm::mat4 mvp = game->calculateMvpMatrix(position, rot, scale);
         shader.use();
         shader.setMatrix("mvp", mvp);
         shader.setVector("spriteColor", this->color);
         // Bind texture
         glActiveTexture(GL_TEXTURE0);
         glBindTexture(GL_TEXTURE_2D, textureID);
         shader.setInt("spriteTexture", 0);       // slot 0 stores a reference to tex data, gpu gets the rgba colors from tex coords and those values go into framebuffer
         shader.setBool("useTexture", true);

         glDepthMask(GL_FALSE); // Don't write to the depth buffer
         // Draw quad
         glBindVertexArray(VAO);
         glDrawArrays(GL_TRIANGLE_FAN, 0, 4); //Triangle fan automatically draws triangles from 4 vertices, taking 4 vertices makes them a quad
         glBindVertexArray(0);

         glDepthMask(GL_TRUE); // Turn it back on for other things


     }

    void Sprite::setTexture(GLuint newTexture) {
         textureID = newTexture;
     }

     void Sprite::cleanup() {
         glDeleteVertexArrays(1, &VAO);
         glDeleteBuffers(1, &VBO);
     }

 }
