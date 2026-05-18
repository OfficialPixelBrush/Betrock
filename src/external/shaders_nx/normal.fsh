#version 300 es
precision highp float;

out vec4 FragColor;

in vec3 currentPosition;
in vec3 Normal;
in vec3 color;
in vec2 textureCoordinate;

uniform sampler2D diffuse0;
uniform vec3 cameraPosition;

void main() {
    vec3 normal = normalize(Normal);
    FragColor = vec4(normal, 1.0);
}
