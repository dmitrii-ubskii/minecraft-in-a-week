#version 460 core
out vec4 fragColor;

in vec2 textureCoords;
in vec4 position;
in vec3 normal;

uniform sampler2D textureAtlas;
uniform vec3 lightDirection;

void main()
{
    fragColor = texture(textureAtlas, textureCoords);
    fragColor *= 0.3 + 0.7 * (0.5 + 0.5 * dot(-normal, lightDirection));

    float dist = length(position.xyz);
    float minFogDist = 120.0;
    float maxFogDist = 160.0;
    float fogFactor = clamp((dist - minFogDist) / (maxFogDist - minFogDist), 0.0, 1.0);
    fragColor = mix(fragColor, vec4(0.5f, 0.8f, 0.9f, 1.f), fogFactor);
}

