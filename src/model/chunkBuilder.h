#pragma once
#include "../world/chunk.h"
#include "../world/world.h"
#include "../render/texture.h"
#include "../render/mesh.h"
#include "../model/model.h"
#include <algorithm>

class ChunkBuilder {
    public:
        ChunkBuilder(Model* blockModel);
        std::vector<Mesh*> build(World* world, uint8_t maxSkyLight = 15);
    private:
        Model* model;
        World* world;
        bool isSurrounded(uint x, uint y, uint z);
        glm::vec2 getBlockTextureOffset(unsigned char blockType, unsigned char blockMetaData);
        uint8_t getBlockModel(unsigned char blockType, uint x, uint y, uint z);
};