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
out float fogFactor;  // Pass fog factor to fragment shader

uniform mat4 cameraMatrix;
uniform mat4 model;
uniform mat4 translation;
uniform mat4 rotation;
uniform mat4 scale;

void main()
{
    //currentPosition = vec3(model * translation * -rotation * scale * vec4(aPos, 1.0f));
    Normal = aNormal;
    color = aColor;
    textureCoordinate = aTexture;

    vec4 viewPosition = cameraMatrix * vec4(aPos, 1.0);

    float distance = length(viewPosition.xyz);  // Distance from the camera
    fogFactor = (distance - 25.0) / (30.0 - 25.0);  // Linear fog factor
    fogFactor = clamp(fogFactor, 0.0, 1.0);       // Clamp between 0 and 1

    gl_Position = viewPosition;
}