#pragma once
#include "../world/chunk.h"
#include "../render/texture.h"
#include "../render/mesh.h"
#include "../model/model.h"
#include <algorithm>

class ChunkBuilder {
    public:
        ChunkBuilder(Model* blockModel);
        Mesh* build(Chunk* pChunk, int chunkX = 0, int chunkZ = 0);
    private:
        Model* model;
        Chunk* chunk;
        bool isSurrounded(uint x, uint y, uint z);
        glm::vec2 getBlockTextureOffset(unsigned char blockType, unsigned char blockMetaData);
        uint8_t getBlockModel(unsigned char blockType, uint x, uint y, uint z);
};