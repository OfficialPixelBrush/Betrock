#version 300 es
precision highp float;

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTexture;
layout (location = 4) in float aSkylight;
layout (location = 5) in float aBlocklight;

out vec3 currentPosition;
out vec3 Normal;
out vec3 color;
out vec2 textureCoordinate;
out float fogFactor;
out vec4 fogColor;

uniform mat4 cameraMatrix;
uniform mat4 model;
uniform float fogDistance;
uniform vec4 externalFogColor;
uniform float maxSkyLight;
uniform bool fullbright;

void main()
{
    Normal = aNormal;
    color = aColor;
    if (!fullbright) {
        color *= vec3(max(aBlocklight, min(aSkylight, maxSkyLight / 15.0)));
    }
    textureCoordinate = aTexture;

    vec4 viewPosition = cameraMatrix * vec4(aPos, 1.0);
    float dist = length(viewPosition.xyz);
    fogFactor = (dist - fogDistance * 0.5) / (fogDistance - fogDistance * 0.5 + 16.0);
    fogFactor = clamp(fogFactor, 0.0, 1.0);
    fogColor = externalFogColor;
    gl_Position = viewPosition;
}
