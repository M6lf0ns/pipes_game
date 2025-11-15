#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#include "Texture.h"
#include <iostream>

namespace gl3 {
    GLuint Texture::LoadTexture(const std::string& path) {
        GLuint textureID;
        glGenTextures(1, &textureID);

        int width, height, nrChannels;
        stbi_set_flip_vertically_on_load(true);
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
        if (!data) {
            std::cerr << "Failed to load texture: " << path << std::endl;
            return 0;
        }

        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Texture filtering/wrapping
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //Clamp horizontally
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); //Clamp vertically
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        std::cout << "Texture width: " << width << ", height: " << height << std::endl;

        stbi_image_free(data); //free the memory allocated when loading tex image
        return textureID;
    }




}
