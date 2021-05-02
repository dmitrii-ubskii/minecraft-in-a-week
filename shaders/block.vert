#version 460 core
layout (location=0) in vec3 aPos;
layout (location=1) in vec2 aTextureCoords;
layout (location=2) in vec3 aNormal;

out vec2 textureCoords;
out vec4 position;
out vec3 normal;
out vec3 shadowCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpace;

void main()
{
    vec4 lightSpacePosition = lightSpace * model * vec4(aPos, 1.0);
    shadowCoords = 0.5 + 0.5 * lightSpacePosition.xyz / lightSpacePosition.w;

    position = view * model * vec4(aPos, 1.0);
    gl_Position = projection * position;
    textureCoords = aTextureCoords;
    normal = aNormal;
}

