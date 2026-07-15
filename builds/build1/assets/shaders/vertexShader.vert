#version 460 core

layout (location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord; // texture coordinates
uniform mat4 mvp;
out vec2 TexCoords; // pass to fragment shader

void main() {
    gl_Position = mvp * vec4(aPos.xyz, 1.0);
    TexCoords = aTexCoord;
}
