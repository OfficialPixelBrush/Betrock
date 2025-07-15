#pragma once
#include "../include/json.h"
#include "../render/mesh.h"
#include <fstream>
#include <sstream>
#include <string>
#include <cstdio>

class Model {
    public:
        Model(const char* file);
        void Draw(Shader& shader, Camera& camera);
        const char* file;
        std::vector<std::unique_ptr<Mesh>> meshes;
        
    private:

        void loadMesh(uint indexMesh);

        void getMeshData();
        std::vector<Texture> getTextures();
};