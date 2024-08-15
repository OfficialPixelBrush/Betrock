#pragma once
#include "../external/json.h"
#include "../render/mesh.h"
#include <fstream>
#include <sstream>
#include <string>
#include <cstdio>


using json = nlohmann::json;

class Model {
    public:
        Model(const char* file);
        void Draw(Shader& shader, Camera& camera);

    private:
        const char* file;
        std::vector<unsigned char> data;
        json Json;

        std::vector<Mesh> meshes;
        std::vector<glm::vec3> translationsMeshes;
        std::vector<glm::quat> rotationsMeshes;
        std::vector<glm::vec3> scalesMeshes;
        std::vector<glm::mat4> matricesMeshes;

        std::vector<std::string> loadedTextureName;
        std::vector<Texture> loadedTexture;

        void loadMesh(uint indexMesh);

        void traverseNode(uint nextNode, glm::mat4 matrix = glm::mat4(1.0f));

        std::vector<unsigned char> getData();
        std::vector<float> getFloats(json accessor);
        std::vector<GLuint> getIndices(json accessor);
        std::vector<Texture> getTextures();

        std::vector<Vertex> assembleVertices (
            std::vector<glm::vec3> positions,
            std::vector<glm::vec3> normals,
            std::vector<glm::vec2> textureUVs
        );

        std::vector<glm::vec2> groupFloatsVec2(std::vector<float> floatVec);
        std::vector<glm::vec3> groupFloatsVec3(std::vector<float> floatVec);
        std::vector<glm::vec4> groupFloatsVec4(std::vector<float> floatVec);
};