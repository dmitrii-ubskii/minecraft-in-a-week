#version 460 core
out vec4 fragColor;

in vec2 textureCoords;
in vec3 normal;

uniform sampler2D textureAtlas;

void main()
{
    vec3 lightDirection = normalize(vec3(2, -3, 1));
    fragColor = texture(textureAtlas, textureCoords);
    fragColor *= 0.3 + 0.7 * (0.5 + 0.5 * dot(-normal, lightDirection));
}

