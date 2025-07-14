#include <algorithm> // For std::copy

#pragma once
#include "mesh.h"

const float dayTimeSkyColor[4] = {float(149/255.0f), float(159/255.0f), float(255/255.0f), 1.0f};
const float dayTimeHorizonColor[4] = {float(178/255.0f), float(197/255.0f), float(255/255.0f), 1.0f};
const float dayTimeGroundColor[4] = {float(56/255.0f), float(60/255.0f), float(188/255.0f), 1.0f};

const float nightTimeColor[4] = {0.439f, 0.651f, 0.918f, 1.0f};
const float sunsetTimeColor[4] = {0.439f, 0.651f, 0.918f, 1.0f};
const float undergroundColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};

class Sky {
    public:
        Mesh* mesh;
        float fogColor[4];
        float skyColor[4];
        Sky(Mesh* mesh);
        void Draw(Shader& shader, Camera& camera);
        void UpdateFog(Shader& shader, int renderDistance);
};