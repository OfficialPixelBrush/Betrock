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

vec4 pointLight() {
    vec3 lightVector = lightPosition - currentPosition;
    float distance = length(lightVector);
    float a = 3.0;
    float b = 0.7;
    float intensity = 1.0f/(a*distance*distance+b*distance+1.0f);

    // Diffuse Lighting
    vec3 normal = normalize(Normal);
    vec3 lightDirection = normalize(lightVector);
    float diffuse = max(dot(normal, lightDirection), 0.0f);
    
    // Calculate Specular Highlight
    float specularLight = 0.5f;
    vec3 viewDirection = normalize(cameraPosition - currentPosition);
    vec3 reflectionDirection = reflect(-lightDirection, normal);
    float specularAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 8);
    float specular = specularAmount * specularLight;

    return (vec4(color,1.0f) * texture(diffuse0, textureCoordinate) * (diffuse * intensity + ambient) + texture(specular0,textureCoordinate).r * specular *intensity) * lightColor;
}

vec4 directionalLight() {
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

    return (vec4(color,1.0f) * texture(diffuse0, textureCoordinate) * (diffuse + ambient) + texture(specular0,textureCoordinate).r * specular) * lightColor;
}

vec4 spotLight() {
    float outerCone = 0.90;
    float innerCone = 0.95;

    // Diffuse Lighting
    vec3 normal = normalize(Normal);
    vec3 lightDirection = normalize(lightPosition - currentPosition);
    float diffuse = max(dot(normal, lightDirection), 0.0f);
    
    // Calculate Specular Highlight
    float specularLight = 0.5f;
    vec3 viewDirection = normalize(cameraPosition - currentPosition);
    vec3 reflectionDirection = reflect(-lightDirection, normal);
    float specularAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 8);
    float specular = specularAmount * specularLight;

    float angle = dot(vec3(0.0f, -1.0f, 0.0f), -lightDirection);
    float intensity = clamp((angle - outerCone) / (innerCone - outerCone), 0.0f, 1.0f);

    return (vec4(color,1.0f) * texture(diffuse0, textureCoordinate) * (diffuse * intensity + ambient) + texture(specular0,textureCoordinate).r * specular *intensity) * lightColor;
}

void main() {
    // The final color of the pixel
    FragColor = pointLight();
}