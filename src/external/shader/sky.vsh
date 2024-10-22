#version 330 core
layout (location = 0) in vec3 aPos;
out vec3 TexCoords;

uniform mat4 cameraMatrix;

void main()
{
    // Transform the vertex position into camera space
    vec4 viewPosition = cameraMatrix * vec4(aPos, 1.0);
    
    // Use only the y-component of the camera space position for TexCoords
    TexCoords = vec3(0.0, viewPosition.y, 0.0);
    
    // Maintain depth properly
    gl_Position = viewPosition;  
}
