#version 460 core

in vec2 TexCoords;
out vec4 fragColor;

uniform vec4 spriteColor;
uniform bool useTexture;
uniform sampler2D spriteTexture;

void main() {
    if (useTexture) {
        // Drawing a Sprite
        fragColor = texture(spriteTexture, TexCoords) * spriteColor;
    } else {
        // Drawing the Trajectory Line (No Texture)
        fragColor = spriteColor;
    }
}