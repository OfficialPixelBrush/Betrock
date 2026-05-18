/**
 * src/render/sky_nx.cpp
 *
 * Switch version of Sky::Draw that also uploads topColor / horizonColor /
 * bottomColor uniforms (required because GLSL ES 3.00 does not allow
 * uniform default-value initialisers that the desktop sky.fsh used).
 *
 * Compile this instead of sky.cpp when PLATFORM_SWITCH is defined;
 * CMakeLists_Switch.txt lists this file in the source list.
 *
 * Everything other than Draw() is identical to sky.cpp.
 */

#include "sky.h"

Sky::Sky(Mesh* mesh) {
    std::copy(std::begin(dayTimeSkyColor),     std::end(dayTimeSkyColor),     skyColor);
    std::copy(std::begin(dayTimeHorizonColor), std::end(dayTimeHorizonColor), fogColor);
    Sky::mesh = mesh;
}

void Sky::Draw(Shader& shader, Camera& camera) {
    // Upload the sky-gradient colours that GLSL ES cannot initialise inline.
    GLint topLoc     = glGetUniformLocation(shader.Id, "topColor");
    GLint horizLoc   = glGetUniformLocation(shader.Id, "horizonColor");
    GLint bottomLoc  = glGetUniformLocation(shader.Id, "bottomColor");

    if (topLoc    >= 0) glUniform3f(topLoc,    158.0f/255.0f, 157.0f/255.0f, 255.0f/255.0f);
    if (horizLoc  >= 0) glUniform3f(horizLoc,  181.0f/255.0f, 198.0f/255.0f, 255.0f/255.0f);
    if (bottomLoc >= 0) glUniform3f(bottomLoc,  53.0f/255.0f,  53.0f/255.0f, 185.0f/255.0f);

    mesh->Draw(shader, camera);
}

void Sky::UpdateFog(Shader& shader, int renderDistance) {
    GLint fogColorLoc    = glGetUniformLocation(shader.Id, "externalFogColor");
    GLint fogDistanceLoc = glGetUniformLocation(shader.Id, "fogDistance");
    glUniform4f(fogColorLoc,    fogColor[0], fogColor[1], fogColor[2], fogColor[3]);
    glUniform1f(fogDistanceLoc, (float)renderDistance);
}
