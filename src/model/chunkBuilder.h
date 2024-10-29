#pragma once
#include "../world/chunk.h"
#include "../world/world.h"
#include "../render/texture.h"
#include "../render/mesh.h"
#include "../model/model.h"
#include "chunkMesh.h"
#include "dummyMesh.h"
#include <algorithm>
#include "../world/blockProperties.h"
#include "../helper.h"

class ChunkBuilder {
    public:
        ChunkBuilder(Model* model, World* world);
        std::vector<DummyMesh> buildChunks(std::vector<Chunk*> chunks, bool smoothLighting, uint8_t maxSkyLight = 15);
        DummyMesh buildChunk(Chunk* chunk, bool smoothLighting, uint8_t maxSkyLight = 15);
    private:
        Mesh* cubeMesh = nullptr;
        Mesh* cachedMesh = nullptr;
        Model* model;
        World* world;
        bool isSurrounded(int x, int y, int z, uint8_t blockType);
        Mesh* getBlockMesh(uint8_t blockType, int x, int y, int z, uint8_t blockMetaData = 0);
        Mesh* searchForMeshName(uint8_t blockType, uint8_t blockMetaData);
        glm::vec2 getBlockTextureOffset(uint8_t blockType, uint8_t blockMetaData);
        void createRectangle(uint8_t blockType, uint8_t blockMetaData, std::vector<Vertex>& vertices, std::vector<GLuint>& indices, const glm::vec3& startPos, const glm::vec3& dimensions, const glm::vec3& normal, const glm::vec3& color);
};