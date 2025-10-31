#include "Mesh.h"
#include "glad/glad.h"

namespace gl3 {
    template<typename T>
    GLuint createBuffer(GLuint bufferType, const std::vector<T> &bufferData) {
        unsigned int buffer = 0;
        glGenBuffers(1, &buffer);
        glBindBuffer(bufferType, buffer);
        glBufferData(bufferType, bufferData.size() * sizeof(T), bufferData.data(), GL_STATIC_DRAW);
        return buffer;
    }

    Mesh::Mesh(const std::vector<float> &vertices, const std::vector<unsigned int> &indices):
            numberOfIndices(indices.size()),
            VBO(createBuffer(GL_ARRAY_BUFFER, vertices)),
            EBO(createBuffer(GL_ELEMENT_ARRAY_BUFFER, indices)) {
    }

    void Mesh::draw() const {
        // Bind VBO
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(0);

        // Bind EBO
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

        // Draw
        glDrawElements(GL_TRIANGLES, numberOfIndices, GL_UNSIGNED_INT, nullptr);
    }

    Mesh::~Mesh() {
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }
}