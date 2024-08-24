#version 330 core

out vec4 FragColor;

in vec3 currentPosition; 
in vec3 Normal; 
in vec3 color;
in vec2 textureCoordinate;

uniform sampler2D diffuse0;
uniform sampler2D specular0;
uniform vec4 lightColor;
uniform vec4 ambient;
uniform vec3 lightPosition;
uniform vec3 cameraPosition;

void main() {
    vec3 normal = normalize(Normal);
    vec3 lightDirection = normalize(vec3(1.0f, 1.0f, 0.0f));

    // Calculate Diffuse light
    float diffuse = max(dot(normal, lightDirection), 0.0f);
    
    // Calculate Specular Highlight
    float specularLight = 0.5f;
    vec3 viewDirection = normalize(cameraPosition - currentPosition);
    vec3 reflectionDirection = reflect(-lightDirection, normal);
    float specularAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 8);
    float specular = specularAmount * specularLight;

    // The final color of the pixel
    FragColor = vec4(-normal,1.0f); //  (vec4(color,1.0f) * texture(diffuse0, textureCoordinate) * (diffuse + ambient) + texture(specular0,textureCoordinate).r * specular) * lightColor;
}