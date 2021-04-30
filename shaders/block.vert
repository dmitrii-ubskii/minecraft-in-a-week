#version 460 core
layout (location=0) in vec3 aPos;
layout (location=1) in vec2 aTextureCoords;
layout (location=2) in vec3 aNormal;

out vec2 textureCoords;
out vec4 position;
out vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    position = view * model * vec4(aPos, 1.0);
    gl_Position = projection * position;
    textureCoords = aTextureCoords;
    normal = aNormal;
}

