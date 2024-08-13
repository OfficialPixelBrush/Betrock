#version 330 core

out vec4 FragColor;

in vec3 color;
in vec2 textureCoordinate;
in vec3 Normal; 
in vec3 currentPosition; 

uniform sampler2D texture0;
uniform vec4 lightColor;
uniform vec3 lightPosition;
uniform vec3 cameraPosition;

void main() {
    float ambient = 0.2f;
    vec3 normal = normalize(Normal);
    vec3 lightDirection = normalize(lightPosition - currentPosition);

    // Calculate Diffuse light
    float diffuse = max(dot(normal, lightDirection), 0.0f);
    
    // Calculate Specular Highlight
    float specularLight = 0.5f;
    vec3 viewDirection = normalize(cameraPosition - currentPosition);
    vec3 reflectionDirection = reflect(-lightDirection, normal);
    float specularAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 8);
    float specular = specularAmount * specularLight;

    // The final color of the pixel
    FragColor = texture(texture0, textureCoordinate) * vec4(color,1.0f) * lightColor * (diffuse + specular + ambient);
}