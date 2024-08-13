#version 330 core
// Input Vertex position
layout (location = 0) in vec3 aPos;

// Input Vertex Color
layout (location = 1) in vec3 aColor;

// Input Texture
layout (location = 2) in vec2 aTexture;

// The color that is output to the fragment shader
out vec3 color;

out vec2 textureCoordinate;

uniform mat4 cameraMatrix;

void main()
{
    gl_Position = cameraMatrix * vec4(aPos, 1.0);
    color = aColor;
    textureCoordinate = aTexture;
}