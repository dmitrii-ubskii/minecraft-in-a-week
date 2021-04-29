#version 460 core
out vec4 fragColor;

in vec2 textureCoords;

uniform sampler2D textureAtlas;

void main()
{
    fragColor = texture(textureAtlas, textureCoords);
    fragColor = fragColor + vec4(1-fragColor.a, 0.0, 1-fragColor.a, 1-fragColor.a);
}

