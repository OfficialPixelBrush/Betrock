#include "sky.h"

Sky::Sky(Model* model) {
    std::copy(std::begin(dayTimeSkyColor), std::end(dayTimeSkyColor), skyColor);
    std::copy(std::begin(dayTimeHorizonColor), std::end(dayTimeHorizonColor), fogColor);
    Sky::model = model;
}

void Sky::Draw(Shader& shader, Camera& camera) {
    model->Draw(shader, camera);
}

void Sky::UpdateFog(Shader& shader, int renderDistance) {
    // Render Distance is given in blocks
    GLint fogColorLoc = glGetUniformLocation(shader.Id, "externalFogColor");
    glUniform4f(fogColorLoc, fogColor[0], fogColor[1], fogColor[2], fogColor[3]);

    // Update fog distance dynamically
    GLint fogDistanceLoc = glGetUniformLocation(shader.Id, "fogDistance");
    glUniform1f(fogDistanceLoc, renderDistance);
}