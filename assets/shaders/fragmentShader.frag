#version 460 core

in vec2 TexCoords;
uniform vec4 color;
out vec4 fragColor;

uniform sampler2D spriteTexture;

void main() {
    fragColor = texture(spriteTexture, TexCoords);
}
