#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

uniform mat4 cameraMatrix;
uniform mat4 model;
uniform float fogDistance;
uniform vec4 externalFogColor;  // Dynamic fog color as a uniform
uniform float maxSkyLight;
uniform bool fullbright;

void main()
{
    TexCoords = aPos;
    gl_Position = projection * view * vec4(aPos, 1.0);
}  