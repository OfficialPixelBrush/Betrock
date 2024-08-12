#version 330 core

out vec4 FragColor;

in vec3 color;

in vec2 textureCoordinate;

uniform sampler2D texture0;

void main() {
    FragColor = texture(texture0, textureCoordinate); //vec4(color,1.0f);
}