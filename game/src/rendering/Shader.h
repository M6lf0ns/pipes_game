#pragma once

#include <string>
#include <filesystem>
#include "glad/glad.h"
#include "glm/glm.hpp"

namespace fs = std::filesystem;

namespace gl3 {
    /// @class Shader
    /// @brief Handles loading, compiling, and managing OpenGL shader programs.
    ///
    /// This class provides an interface to load vertex and fragment shaders from disk,
    /// link them into a shader program, and set uniform values during the draw call.
    class Shader {
    public:
        /// @brief Constructs and compiles a shader program.
        /// @param vertexShaderPath Path to the vertex shader source file.
        /// @param fragmentShaderPath Path to the fragment shader source file.
        Shader(const fs::path &vertexShaderPath, const fs::path &fragmentShaderPath);

        /// @brief Destructor: Frees shader program resources.
        ~Shader();

        /// @brief Deleted copy constructor to prevent accidental duplication of OpenGL resources.
        Shader(const Shader &shader) = delete;

        /// @brief Move constructor: Transfers ownership of the shader program.
        Shader(Shader &&other) noexcept {
            std::swap(this->shaderProgram, other.shaderProgram);
            std::swap(this->vertexShader, other.vertexShader);
            std::swap(this->fragmentShader, other.fragmentShader);
        }

        /// @brief Sets an integer uniform value in the shader.
        /// @param name The name of the uniform in the shader code.
        /// @param value The integer value to set.
        void setInt(const std::string& name, int value);

        /// @brief Sets a 4x4 matrix uniform value.
        /// @param uniformName The name of the uniform in the shader code.
        /// @param matrix The glm::mat4 matrix to pass.
        void setMatrix(const std::string &uniformName, glm::mat4 matrix) const;

        /// @brief Sets a 4-component vector uniform value.
        /// @param uniformName The name of the uniform in the shader code.
        /// @param vector The glm::vec4 vector to pass.
        void setVector(const std::string &uniformName, glm::vec4 vector) const;

        /// @brief Sets a boolean uniform value.
        /// @param name The name of the uniform in the shader code.
        /// @param value The boolean value to set.
        void setBool(const std::string &name, bool value) const;

        /// @brief Activates the shader program for rendering.
        void use() const;

    private:
        /// @brief Internal helper to load and compile an individual shader stage.
        unsigned int loadAndCompileShader(GLuint shaderType, const fs::path &shaderPath);

        /// @brief Internal helper to read shader source code from a file.
        std::string readText(const fs::path &filePath);

        unsigned int shaderProgram;   ///< OpenGL ID of the linked program.
        unsigned int vertexShader;    ///< OpenGL ID of the vertex shader.
        unsigned int fragmentShader;  ///< OpenGL ID of the fragment shader.
    };
}