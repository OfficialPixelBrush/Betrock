#version 330 core

in vec3 TexCoords;
out vec4 FragColor;

// You can tweak these colors to get your desired sky appearance
uniform vec3 topColor = vec3(0.529, 0.808, 0.922);   // Sky blue
uniform vec3 bottomColor = vec3(0.984, 0.741, 0.482); // Light orange / horizon

void main()
{
    // Normalize y from -1 (bottom) to +1 (top), then remap to 0..1
    //float t = clamp(TexCoords.y * 0.5 + 0.5, 0.0, 1.0);
    //vec3 color = mix(bottomColor, topColor, t);
    FragColor = vec4(TexCoords, 1.0);
}
