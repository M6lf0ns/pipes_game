#pragma once
#include <glad/glad.h>
#include <string>

namespace gl3 {
class Texture {

public:
    static GLuint LoadTexture(const std::string& path);

    int texWidth, texHeight;

};
};


