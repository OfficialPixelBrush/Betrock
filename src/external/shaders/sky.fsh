#version 330 core

in vec3 TexCoords;
out vec4 FragColor;

// You can tweak these colors to get your desired sky appearance
uniform vec3 topColor       = vec3(158.0/255.0, 157.0/255.0, 255.0/255.0);
uniform vec3 horizonColor   = vec3(181.0/255.0, 198.0/255.0, 255.0/255.0); 
uniform vec3 bottomColor    = vec3( 53.0/255.0,  53.0/255.0, 185.0/255.0);

uniform float timeOfDay;

void main()
{
    vec3 dir = normalize(TexCoords);

    // Calculate angle from vertical (dot product with up vector)
    float cosTheta = dot(dir, vec3(0.0, 1.0, 0.0)); // -1 (down) to +1 (up)
    float t = cosTheta + 0.1;
    //float t = (cosTheta + 1.0) * 0.5; // Remap to 0.0 - 1.0

    // Blend bottom → horizon → top using t
    vec3 color = vec3(1.0,0.0,0.0);

    if (t < -0.01) {
        float factor = smoothstep(-0.3, -0.01, t);
        color = mix(bottomColor, horizonColor, factor*factor*factor*factor);
    } else if (t < 0.01) {
        color = horizonColor;
    } else {
        float factor = smoothstep(0.01, 0.3, t);
        float factor2 = smoothstep(0.0, 1.0, t);
        color = mix(horizonColor, topColor, factor*factor*factor*factor);
        color = mix(horizonColor, color, factor2);
    }

    FragColor = vec4(color*timeOfDay, 1.0);
}
