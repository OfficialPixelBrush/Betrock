#version 300 es
precision highp float;

in vec3 TexCoords;
out vec4 FragColor;

// These cannot have initializers in GLSL ES — set them from C++ code
// or just hardcode them here.
uniform vec3 topColor;
uniform vec3 horizonColor;
uniform vec3 bottomColor;
uniform float timeOfDay;

void main()
{
    vec3 dir = normalize(TexCoords);
    float cosTheta = dot(dir, vec3(0.0, 1.0, 0.0));
    float t = (cosTheta + 1.0) * 0.5;

    vec3 col = bottomColor;
    if (t < 0.5) {
        float factor = smoothstep(0.45, 0.5, t);
        col = mix(bottomColor, horizonColor, factor);
    } else {
        float factor = smoothstep(0.55, 0.6, t);
        col = mix(horizonColor, topColor, factor);
    }

    FragColor = vec4(col * timeOfDay, 1.0);
}
