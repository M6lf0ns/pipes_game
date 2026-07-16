#pragma once
#include <glad/glad.h>
#include <string>

namespace gl3 {
    /// @class Texture
    /// @brief Handles loading and management of OpenGL texture resources.
class Texture {

public:
    /// @brief Loads a texture from an image file into OpenGL memory.
    ///
    /// This function handles the file I/O, texture parameter setup,
    /// and generation of the OpenGL texture ID.
    ///
    /// @param path The file system path to the image file (e.g., .png or .jpg).
    /// @return The generated OpenGL texture ID.

    static GLuint LoadTexture(const std::string& path);


    int texWidth, texHeight; ///< The width and height of the loaded texture in pixels.

};
};


