#pragma once
#include "../world/chunk.h"
#include "../render/texture.h"
#include "../render/mesh.h"
#include "../model/model.h"

class ChunkBuilder {
    public:
        ChunkBuilder(Model* blockModel);
        Mesh* build(Chunk* pChunk, int chunkX = 0, int chunkZ = 0);
    private:
        Model* model;
        Chunk* chunk;
        bool isSurrounded(uint x, uint y, uint z);
        glm::vec2 getBlockTextureOffset(unsigned char blockType);
        Mesh* getBlockModel(unsigned char blockType, uint x, uint y, uint z);
};