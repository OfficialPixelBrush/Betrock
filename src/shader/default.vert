#version 330 core
// Input Vertex position
layout (location = 0) in vec3 aPos;

// Input Vertex Color
layout (location = 1) in vec3 aColor;

// Input Texture
layout (location = 2) in vec2 aTexture;

// The color that is output to the fragment shader
out vec3 color;

out vec2 textureCoordinate;

// Used to scale the verts
uniform float scale;

void main()
{
    gl_Position = vec4(aPos.x + aPos.x*scale, aPos.y + aPos.y*scale, aPos.z, 1.0 + aPos.z*scale);
    color = aColor;
    textureCoordinate = aTexture;
}