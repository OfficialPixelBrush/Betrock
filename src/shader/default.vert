#version 330 core
// Input Vertex position
layout (location = 0) in vec3 aPos;
// Input Vertex Color
layout (location = 1) in vec3 aColor;
// Input Texture
layout (location = 2) in vec2 aTexture;
// Input normal
layout (location = 3) in vec3 aNormal;

// The color that is output to the fragment shader
out vec3 color;
out vec2 textureCoordinate;
out vec3 Normal;
out vec3 currentPosition;

uniform mat4 cameraMatrix;
uniform mat4 model;

void main()
{
    currentPosition = vec3(model * vec4(aPos, 1.0f));
    gl_Position = cameraMatrix * vec4(currentPosition, 1.0);
    color = aColor;
    textureCoordinate = aTexture;
    Normal = aNormal;
}