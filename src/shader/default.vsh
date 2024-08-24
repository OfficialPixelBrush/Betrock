#version 330 core
// Input Vertex position
layout (location = 0) in vec3 aPos;
// Input normal
layout (location = 1) in vec3 aNormal;
// Input Vertex Color
layout (location = 2) in vec3 aColor;
// Input Texture
layout (location = 3) in vec2 aTexture;

// The color that is output to the fragment shader
out vec3 currentPosition;
out vec3 Normal;
out vec3 color;
out vec2 textureCoordinate;

uniform mat4 cameraMatrix;
uniform mat4 model;
uniform mat4 translation;
uniform mat4 rotation;
uniform mat4 scale;

void main()
{
    currentPosition = vec3(model * translation * -rotation * scale * vec4(aPos, 1.0f));
    Normal = aNormal;
    color = aColor;
    textureCoordinate = mat2(0.0, -1.0, 1.0, 0.0) * aTexture;
    
    gl_Position = cameraMatrix * vec4(currentPosition, 1.0);
}