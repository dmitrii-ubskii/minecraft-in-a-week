#version 460 core
layout (location=0) in vec3 aPos;
layout (location=1) in vec2 aTextureCoords;

uniform vec2 position;
uniform vec2 scale;
out vec2 textureCoords;

void main()
{
    gl_Position = vec4(aPos.xy * scale + position, aPos.z, 1.0);
    textureCoords = aTextureCoords;
}

