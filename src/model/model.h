#pragma once
#include "../external/json.h"
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
        std::vector<Mesh> meshes;
        
    private:

        void loadMesh(uint indexMesh);

        void getMeshData();
        std::vector<Texture> getTextures();
};