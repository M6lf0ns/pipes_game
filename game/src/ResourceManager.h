#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H
#pragma once
#include <glad/glad.h>

#include <map>
#include <string>

#include "rendering/Texture.h"
#include "rendering/Shader.h"


namespace gl3 {
    class ResourceManager {
        public:
        static std::map<std::string, Shader> shaders;
        static std::map<std::string, Texture> textures;

        static Shader LoadShader(std::string vertexShader, std::string fragmentShader);

        static Shader GetShader(std::string vertexShader, std::string fragmentShader);

        static Texture LoadTexture(std::string texturePath);

        static void Clear();

    private:
        ResourceManager() {}

        static Shader loadShaderFromFile(std::string vertexShader, std::string fragmentShader);

        static Texture loadTextureFromFile(std::string texturePath);






    };
}


#endif

