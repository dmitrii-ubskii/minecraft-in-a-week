#version 460 core
out vec4 fragColor;

in vec2 textureCoords;

uniform sampler2D image;

void main()
{
    fragColor = texture(image, textureCoords);
}

