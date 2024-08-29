#version 330 core

out vec4 FragColor;

in vec3 currentPosition; 
in vec3 Normal;
in vec3 color;
in vec2 textureCoordinate;

uniform sampler2D diffuse0;
// TODO: uniform vec3 blockLight;
uniform vec3 cameraPosition;

void main() {
    vec3 normal = normalize(Normal);
    vec4 texColor = texture(diffuse0, textureCoordinate);

    // Calculate light
    float lighting = 1.0f;
    
    if (normal.y > 0.0f) {
        lighting = 1.0f;
    } else if (normal.y < 0.0f) {
        lighting = 0.5f;
    } else if (normal.x != 0) {
        lighting = 0.6;
    } else if (normal.z != 0) {
        lighting = 0.8;
    } else {
        lighting = 1.0f;
    }

    // The final color of the pixel
    if(texColor.a < 0.1)
        discard;
    FragColor = vec4(color,1.0f) * vec4(lighting,lighting,lighting,1.0f) * texColor;
}