#pragma once
#include "../world/chunk.h"
#include "../render/texture.h"
#include "../render/mesh.h"

class ChunkBuilder {
    public:
        Mesh* build(Mesh* blockModel, Chunk* chunk, std::vector <Texture> tex);
};