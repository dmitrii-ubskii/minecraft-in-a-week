#version 460 core
out vec4 fragColor;

in vec2 textureCoords;
in vec4 position;
in vec3 normal;
in vec3 shadowCoords;

uniform sampler2D textureAtlas;
uniform sampler2D shadowMap;
uniform vec3 lightDirection;
uniform float renderDistance;

void main()
{
    fragColor = texture(textureAtlas, textureCoords);
    
    float lightIntensity = 0.5 + 0.5 * dot(-normal, lightDirection);

    float lightBarrierDepth = texture(shadowMap, shadowCoords.xy).r;
    float lightDepth = shadowCoords.z;

    float bias = max(3e-4 * (1.0 - dot(normal, lightDirection)), 2e-5); 
    if (lightDepth - bias > lightBarrierDepth)
    {
        lightIntensity = 0.0;
    }

    fragColor *= 0.3 + 0.7 * lightIntensity;
    fragColor.a = 1;

    float dist = length(position.xyz);
    float minFogDist = 0.8 * renderDistance;
    float maxFogDist = renderDistance;
    float fogFactor = clamp((dist - minFogDist) / (maxFogDist - minFogDist), 0.0, 1.0);
    fragColor = mix(fragColor, vec4(0.5f, 0.8f, 0.9f, 1.f), fogFactor);
}

