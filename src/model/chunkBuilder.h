#pragma once
#include "../world/chunk.h"
#include "../world/world.h"
#include "../render/texture.h"
#include "../render/mesh.h"
#include "../model/model.h"
#include "chunkMesh.h"
#include <algorithm>

class ChunkBuilder {
    public:
        ChunkBuilder(Model* model, World* world);
        std::vector<ChunkMesh*> buildChunks(std::vector<Chunk*> chunks, uint8_t maxSkyLight = 15);
        ChunkMesh* buildChunk(Chunk* chunk, uint8_t maxSkyLight = 15);
    private:
        Model* model;
        World* world;
        bool isSurrounded(int x, int y, int z);
        glm::vec2 getBlockTextureOffset(unsigned char blockType, unsigned char blockMetaData);
        uint8_t getBlockModel(unsigned char blockType, int x, int y, int z);
};