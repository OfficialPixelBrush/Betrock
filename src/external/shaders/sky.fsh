#version 330 core
out vec4 FragColor;

in vec3 TexCoords;  // The y component from the camera's position

vec3 topColor = vec3(1.0, 1.0, 1.0);     // The color at the top of the sky (white)
vec3 bottomColor = vec3(0.0, 0.0, 0.0);  // The color at the bottom of the sky (black)

void main()
{
    // Normalize TexCoords.y from [-1, 1] to [0, 1]
    float factor = clamp((TexCoords.y + 1.0) / 2.0, 0.0, 1.0);
    vec3 gradientColor = mix(bottomColor, topColor, factor);  // Interpolate between bottom and top colors
    FragColor = vec4(gradientColor, 1.0); // Set the final color with full opacity
}
