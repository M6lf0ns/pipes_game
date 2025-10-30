#version 460 core

    layout (location = 0) in vec3 aPos;
    uniform mat4 mvp;
    void main() {
        gl_Position = mvp * vec4(aPos.xyz, 1.0); // <-- now using the mvp matrix to transform the position

    }

