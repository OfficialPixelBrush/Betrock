#version 330 core

out vec4 FragColor;

in vec3 currentPosition; 
in vec3 Normal; 
in vec3 color;
in vec2 textureCoordinate;

uniform sampler2D diffuse0;
uniform vec3 cameraPosition;

void main() {
    vec3 normal = normalize(Normal);
    float dotProd = dot(normal,vec3(1.0,0.5,1.0));

    // The final color of the pixel
    FragColor = vec4(dotProd,dotProd,dotProd,1.0f);
}