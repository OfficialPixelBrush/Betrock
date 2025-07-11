#version 330 core
// Input Vertex position
layout (location = 0) in vec3 aPos;
// Input normal
layout (location = 1) in vec3 aNormal;
// Input Vertex Color
layout (location = 2) in vec3 aColor;
// Input Texture
layout (location = 3) in vec2 aTexture;
// Skylight 
layout (location = 4) in float aSkylight;
// Blocklight 
layout (location = 5) in float aBlocklight;

// The color that is output to the fragment shader
out vec3 currentPosition;
out vec3 Normal;
out vec3 color;
out vec2 textureCoordinate;
out float fogFactor;  // Pass fog factor to fragment shader
out vec4 fogColor;

uniform mat4 cameraMatrix;
uniform mat4 model;
uniform mat4 translation;
uniform mat4 rotation;
uniform mat4 scale;
uniform float fogDistance;
uniform vec4 externalFogColor;  // Dynamic fog color as a uniform
uniform float maxSkyLight;

void main()
{
    //currentPosition = vec3(model * translation * -rotation * scale * vec4(aPos, 1.0f));
    Normal = aNormal;
    color = aColor*vec3(max(aBlocklight,min(aSkylight,maxSkyLight/15.0))); //vec3(min(aSkylight,maxSkyLight/15.0));
    textureCoordinate = aTexture;

    vec4 viewPosition = cameraMatrix * vec4(aPos, 1.0);

    float distance = length(viewPosition.xyz);  // Distance from the camera
    fogFactor = (distance - fogDistance * 0.5f) / (fogDistance - fogDistance * 0.5f + 16.0f);
    fogFactor = clamp(fogFactor, 0.0, 1.0);  // Clamp between 0 and 1

    fogColor = externalFogColor;  // Use the dynamically set fog color

    gl_Position = viewPosition;
}