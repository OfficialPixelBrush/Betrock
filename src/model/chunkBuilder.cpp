#include "chunkBuilder.h"
#include <stdexcept>  // For std::runtime_error

#include <stdexcept>  // For std::runtime_error

const float lightArray[16] = {0.035f, 0.044f, 0.055f, 0.069f, 0.086f, 0.107f, 0.134f, 0.168f, 0.21f, 0.262f, 0.328f, 0.41f, 0.512f, 0.64f, 0.8f, 1.0f};

bool ChunkBuilder::isSurrounded(int x, int y, int z, uint8_t blockIndex) {
    bool onlySurroundedBySame = false;
    if (blockIndex >= 8 && blockIndex <= 11) {
        onlySurroundedBySame = true;
    }
    // Cache block pointers
    Block* blockPointers[6] = {
        world->getBlock(x - 1, y, z),
        world->getBlock(x + 1, y, z),
        world->getBlock(x, y - 1, z),
        world->getBlock(x, y + 1, z),
        world->getBlock(x, y, z - 1),
        world->getBlock(x, y, z + 1)
    };

    uint8_t blocks[6] = { 0 };
    for (int i = 0; i < 6; i++) {
        if (blockPointers[i] != nullptr) {
            blocks[i] = blockPointers[i]->getBlockType();
        } else {
            return false;
        }
    }

    // Check if any adjacent block is transparent
    for (int i = 0; i < 6; ++i) {
        if (isTransparent(blocks[i])) {
            return false;
        }
    }

    if (onlySurroundedBySame) {
        // Check if all adjacent blocks have a block type
        for (int i = 0; i < 6; ++i) {
            if (blocks[i] == blockIndex) {
                return false;
            }
        }
    } else {
    // Check if all adjacent blocks have a block type
        for (int i = 0; i < 6; ++i) {
            if (blocks[i] == 0 || isPartialBlock(blocks[i])) {
                return false;
            }
        }
    }

    return true;
}

glm::vec3 getBiomeBlockColor(uint8_t& blockType, uint8_t& blockMetaData, glm::vec3& normal) {
    glm::vec3 color = glm::vec3(0.57, 0.73, 0.34);
    // Biome Colored
    if ((blockType == GRASS && normal.y > 0.0f) || (blockType == TALLGRASS)) {
        return color;
    } else if (blockType == LEAVES) {
        // Spruce
        if (blockMetaData == 1) {
            return glm::vec3(0.38,0.6,0.38);    
        // Birch
        } else if (blockMetaData == 2) {
            return glm::vec3(0.502,0.655,0.333);
        } else {
            return color;
        }
    }
    // Ignore Biome Coloring
    return glm::vec3(1.0f,1.0f,1.0f);
}

ChunkBuilder::ChunkBuilder(Model* model, World* world) {
    ChunkBuilder::model = model;
    ChunkBuilder::world = world;
    ChunkBuilder::cubeMesh = searchForMeshName(0,0);
}

float getLighting(World* world, int x, int y, int z, glm::vec3 normal, uint8_t maxSkyLight) {
    // Array for light values

    try {
        // Calculate adjacent block coordinates
        int adjX = x + static_cast<int>(normal.x);
        int adjY = y + static_cast<int>(normal.y);
        int adjZ = z + static_cast<int>(normal.z);

        // Get the adjacent block
        Block* adjacentBlock = world->getBlock(adjX, adjY, adjZ);
        if (adjacentBlock == nullptr) {
            throw std::runtime_error("Adjacent block pointer is null.");
        }

        // Get lighting values from the adjacent block
        int light = adjacentBlock->getBlockLight() + std::min(adjacentBlock->getSkyLight(), maxSkyLight);

        // Ensure light index is within bounds
        if (light < 0) {
            light = 0;
        } else if (light > 15) {
            light = 15;
        }

        return lightArray[light];

    } catch (const std::exception& e) {
        return 1.0f;  // Return a default value in case of an error
    }
}

bool isHidden(World* world, int x, int y, int z, Block* currentBlock, glm::vec3 normal) {
    if (!currentBlock) {
        return true;
    }
    // Calculate adjacent block coordinates
    int adjX = x + static_cast<int>(normal.x);
    int adjY = y + static_cast<int>(normal.y);
    int adjZ = z + static_cast<int>(normal.z);

    // Get the adjacent block
    Block* adjacentBlock = world->getBlock(adjX, adjY, adjZ);
    // No adjacent Block, not hidden
    if (!adjacentBlock) {
        return true;
    }
    uint8_t cbType = currentBlock->getBlockType();
    uint8_t cbMeta = currentBlock->getMetaData();
    uint8_t abType = adjacentBlock->getBlockType();
    uint8_t abMeta = adjacentBlock->getMetaData();

    // Snow Layer optimization
    if (cbType == SNOW_LAYER && !isTransparent(abType) && normal.y < 0.0) {
        return true;
    }

    if (abType == SNOW_LAYER && !isTransparent(cbType) && normal.y > 0.0) {
        return true;
    }

    // If it's the same as the checking block
    if (isFluid(cbType) && abType == ICE) {
        return true;
    }

    // Prevent top faces from getting culled on Liquids
    if (isFluid(cbType) && cbType != abType && normal.y > 0.0) {
        return false;
    }

    if (isTransparent(cbType) || isPartialBlock(cbType) || isTransparent(abType) || isPartialBlock(abType)) {
        if (cbType == abType) {
            if (isFluid(cbType)) {
                return true;
            }
            if ((cbType == ICE || cbType == SNOW_LAYER || cbType == GLASS) && cbType != LEAVES) {
                return true;
            }
            return false;
        } else if (isTransparent(abType) || isPartialBlock(abType)) {
            return false;
        }
        return true;
    }
    return true;
}

std::vector<std::string> splitString(const std::string& str, char delimiter) {
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string token;
    
    while (std::getline(ss, token, delimiter)) {
        result.push_back(token);
    }
    
    return result;
}

Mesh* ChunkBuilder::searchForMeshName(uint8_t blockType, uint8_t blockMetaData) {
    std::vector<std::string> compareTo;
    // Check Cached Mesh first to save time
    if (cachedMesh != nullptr) {
        compareTo = splitString(cachedMesh->name,'_');
        if (blockType == std::stoi(compareTo[0]) && blockMetaData == std::stoi(compareTo[1])) {
            return cachedMesh;
        }
    }

    for (auto& m : model->meshes) {
        compareTo = splitString(m.name,'_');
        if (blockType == std::stoi(compareTo[0])) {
            // TODO: Temp while some metadata situations aren't accounted for
            if (blockType == LEAVES ||
                blockType == LOG ||
                blockType == TALLGRASS ||
                blockType == TORCH ||
                blockType == STONE_STAIRS ||
                blockType == OAK_STAIRS ||
                blockType == WOOL ||
                blockType == PISTON ||
                blockType == TRAPDOOR ||
                blockType == WOODEN_DOOR ||
                blockType == IRON_DOOR ||
                blockType == LADDER ||
                blockType == BED
            ) {
                if (blockMetaData == std::stoi(compareTo[1])) {
                    return &m;
                } else {
                    continue;
                }
            }
        }
    }
    return &model->meshes[0];
}

Mesh* ChunkBuilder::getBlockMesh(uint8_t blockType, int x, int y, int z, uint8_t blockMetaData) {
    if (blockType == AIR) {
        return nullptr;
    }
    /*
    // Snow
    if (blockType == GRASS) {
        Block* b = world->getBlock(x,y+1,z);
        if (b && b->getBlockType() == SNOW_LAYER) {
            specialQuery = "Snow";
        }
    }

    // Leaves (removes decay data)
    if (blockType == LEAVES) {
        blockMetaData &= 0x3;
    }*/
    
    if (isCube(blockType)) {
        //std::vector<Vertex> vertices = {};
        //std::vector<GLuint> indices = {};
        //std::vector<GLuint> texUV = {};
        //return new Mesh()
        // TODO: Proper textures
        /*for (auto& v : cubeMesh->vertices) {
            v.textureUV = getBlockTextureOffset(blockType,blockMetaData);
        }*/
        return cubeMesh;
    } else {
        // Blocks that should ignore rotation data
        if (blockType == BED  || blockType == WOODEN_DOOR || blockType == IRON_DOOR) {
            blockMetaData &= 0x8;
        }
        
        return searchForMeshName(blockType,blockMetaData);
    }
}
glm::vec2 ChunkBuilder::getBlockTextureOffset(uint8_t blockType, uint8_t blockMetaData) {
    float x = 0;
    float y = 0;
    const float divisor = 0.0625f;
                                                                                                                         // v Some sort of missing entry here!                   //v 50: torch
    uint8_t xBlock [] = { 0, 1, 0, 2, 0, 4,15, 1,15,15,15,15, 2, 3, 0, 1, 2, 4, 4, 0, 1, 0, 0,14, 0,10, 7, 3, 3,10,11, 7, 0, 7,12,11, 0,13,12,13,12, 7, 6, 5, 5, 7, 8, 3, 4, 5, 0,15, 1, 4,11, 5, 2, 8,11,15, 7,12,13, 4, 1, 3, 0, 0, 4, 0, 1, 2, 4, 3, 3, 3, 3, 1, 2, 3, 2, 6, 8, 9,11, 4, 7};
    uint8_t yBlock [] = { 0, 0, 0, 0, 1, 0, 0, 1,13,13,15,15, 1, 1, 2, 2, 2, 1, 3, 3, 3,10, 9, 2,12, 4, 8,11,12, 6, 0, 2, 0, 3, 6, 6, 4, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 2, 2, 2, 5, 1, 4, 0, 1,10, 3, 1, 3, 5, 5, 2, 3, 0, 5, 5, 8, 1, 0, 6, 0, 5, 0, 3, 3, 7, 6, 0, 4, 4, 4, 4, 4, 4, 4, 0, 7};
    return glm::vec2(float(xBlock[blockType])*divisor,-float(yBlock[blockType])*divisor);
}

float getAmbientOcclusion(World* world, glm::vec3& position, glm::vec3& vertexPosition, glm::vec3& normal) {
    Block* b1;
    Block* b2;
    Block* bc;
    glm::vec3 off1;
    glm::vec3 off2;
    glm::vec3 offc;
    if (normal.x != 0.0f) {
        off1 = position + glm::vec3(vertexPosition.x, vertexPosition.y,-vertexPosition.z);
        off2 = position + glm::vec3(vertexPosition.x,-vertexPosition.y, vertexPosition.z);
        offc = position + glm::vec3(vertexPosition.x, vertexPosition.y, vertexPosition.z);
    }
    if (normal.y != 0.0f) {
        off1 = position + glm::vec3( vertexPosition.x,vertexPosition.y,-vertexPosition.z);
        off2 = position + glm::vec3(-vertexPosition.x,vertexPosition.y, vertexPosition.z);
        offc = position + glm::vec3( vertexPosition.x,vertexPosition.y, vertexPosition.z);
    }
    if (normal.z != 0.0f) {
        off1 = position + glm::vec3(-vertexPosition.x, vertexPosition.y,vertexPosition.z);
        off2 = position + glm::vec3( vertexPosition.x,-vertexPosition.y,vertexPosition.z);
        offc = position + glm::vec3( vertexPosition.x, vertexPosition.y,vertexPosition.z);
    }
    uint8_t b1Type = 0; 
    uint8_t b2Type = 0;
    uint8_t bcType = 0;
    int side1  = 0;
    int side2  = 0;
    int corner = 0;
    // Get Blocks
    b1 = world->getBlock(floor(off1.x), floor(off1.y), floor(off1.z));
    if (b1) {
        b1Type = b1->getBlockType();
    }
    b2 = world->getBlock(floor(off2.x), floor(off2.y), floor(off2.z));
    if (b2) {
        b2Type = b2->getBlockType();
    }
    bc = world->getBlock(floor(offc.x), floor(offc.y), floor(offc.z));
    if (bc) {
        bcType = bc->getBlockType();
    }
    if (b1 && !isTransparent(b1Type) && !isPartialBlock(b1Type)) { side1  = b1Type; }
    if (b2 && !isTransparent(b2Type) && !isPartialBlock(b2Type)) { side2  = b2Type; }
    if (bc && !isTransparent(bcType) && !isPartialBlock(bcType)) { corner  = bcType; }

    // Convert block existence to integer (1 if block exists, 0 otherwise)
    int side1Int = side1 ? 1 : 0;
    int side2Int = side2 ? 1 : 0;
    int cornerInt = corner ? 1 : 0;

    // Apply vertexAO formula
    float ao = 4.0f - static_cast<float>(side1Int + side2Int + cornerInt);

    // Normalize the AO level (between 0 and 1)
    ao /= 4.0f;

    return ao;
}

float near(float number) {
    if (number > 0.0) {
        return std::ceil(number);
    }
    if (number < 0.0) {
        return std::floor(number);
    }
    return 0.0;
}

int nearInt(float number) {
    int num = int(near(number));
    //std::cout << number << "->" << num << std::endl;
    return num;
}

#include <format>
std::string printTripleFloat(float x, float y, float z) {
    return std::format("{}, {}, {}", x,y,z);
}

std::string printTripleInt(int x, int y, int z) {
    return std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z);
}

float getSmoothLighting(World* world, int& x, uint& y, int& z, glm::vec3& vertexPosition, glm::vec3& normal, uint8_t& maxSkyLight) {
    uint8_t light = 0;
    uint8_t finalLight = 0.0;
    uint8_t relevantLights = 0;
    Block* blocks[4] = { nullptr };
    // Get the adjacent blocks along face
    if (normal.y != 0.0) {
        blocks[0] = world->getBlock(x,y+1,z);
        blocks[1] = world->getBlock(x,y+1,z+nearInt(vertexPosition.z));
        blocks[2] = world->getBlock(x+nearInt(vertexPosition.x),y+1,z);
        blocks[3] = world->getBlock(x+nearInt(vertexPosition.x),y+1,z+nearInt(vertexPosition.z));
    }
    
    for (auto b : blocks) {
        if (b && (isTransparent(b->getBlockType()) || isPartialBlock(b->getBlockType()))) {
            // Air is transparent, so we can ignore it too
            light = std::max(light,b->getBlockLight());
            light = std::max(light, std::min(b->getSkyLight(), maxSkyLight));
        }
    }
    return lightArray[light];
}

glm::vec3 rotateVertexAroundOrigin(glm::vec3 vertexPosition, float angle, glm::vec3 axis) {
    // Create a rotation matrix for the given angle and axis
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(angle), axis);
    
    // Apply the rotation to the vertex position
    glm::vec4 rotatedPosition = rotationMatrix * glm::vec4(vertexPosition, 1.0f);
    
    // Return the rotated vertex position as a vec3
    return glm::vec3(rotatedPosition);
}

glm::vec3 rotateNormalAroundOrigin(glm::vec3 normal, float angle, glm::vec3 axis) {
    // Create a rotation matrix for the given angle and axis
    glm::mat3 rotationMatrix = glm::mat3(glm::rotate(glm::mat4(1.0f), glm::radians(angle), axis));
    
    // Apply the rotation to the normal
    glm::vec3 rotatedNormal = rotationMatrix * normal;
    
    // Return the rotated normal
    return rotatedNormal;
}



std::vector<DummyMesh> ChunkBuilder::buildChunks(std::vector<Chunk*> chunks, bool smoothLighting, uint8_t maxSkyLight) {
    std::vector<DummyMesh> meshes;
    for (auto c : chunks) {
        meshes.push_back(buildChunk(c,smoothLighting,maxSkyLight));
    }
    return meshes;
}

void rotateBlockAccordingToMetaData(glm::vec3& vertPos, glm::vec3& normal, uint8_t& blockType, uint8_t& blockMetaData) {
    bool changed = false;
    float angle = 0;
    glm::vec3 rotationAxis = glm::vec3(0.0,1.0,0.0);
    // Dynamic rotations
    if (blockType == STANDING_SIGN) {
        changed = true;
        angle = (16-float(blockMetaData))*22.5f;
    // Group I
    } else if (blockType == PUMPKIN || blockType == LIT_PUMPKIN) {
        changed = true;
        switch(blockMetaData & 0x07) {
            default: // North
                break;
            case 0: // South
                angle = 180;
                break;
            case 3: // East
                angle = -90;
                break;
            case 1: // West
                angle = 90;
                break;
        }
    // Group H
    } else if (blockType == WOODEN_DOOR || blockType == IRON_DOOR) {
        changed = true;
        switch(blockMetaData & 0x07) {
            default: // North
                break;
            case 1: // South
                angle = 180;
                break;
            case 0: // East
                angle = -90;
                break;
            case 2: // West
                angle = 90;
                break;
        }
    // Group C
    } else if (blockType == BED) {
        changed = true;
        switch(blockMetaData & 0x07) {
            default: // North
                break;
            case 2: // South
                angle = 180;
                break;
            case 1: // East
                angle = -90;
                break;
            case 3: // West
                angle = 90;
                break;
        }
    // Group E
    } else if (blockType == LADDER) {
        changed = true;
        switch(blockMetaData) {
            default: // North
                break;
            case 3: // South
                angle = 180;
                break;
            case 5: // East
                angle = -90;
                break;
            case 4: // West
                angle = 90;
                break;
        }
    }
    if (changed) {
        vertPos = rotateVertexAroundOrigin(vertPos, angle, rotationAxis);
        normal = rotateNormalAroundOrigin(normal, angle, rotationAxis);
    }
}

glm::vec3 getFluidVertexOffset(uint8_t& blockMetaData) {
    switch(blockMetaData) {
        case 2:
            return glm::vec3(0.0,-1.0+0.75 ,0.0);
        case 3:
            return glm::vec3(0.0,-1.0+0.625,0.0);
        case 4:
            return glm::vec3(0.0,-1.0+0.5  ,0.0);
        case 5:
            return glm::vec3(0.0,-1.0+0.375,0.0);
        case 6:
            return glm::vec3(0.0,-1.0+0.25 ,0.0);
        case 7:
            return glm::vec3(0.0,-1.0+0.125,0.0);
        case 8:
            return glm::vec3(0.0,0.0,0.0);
        default:
            return glm::vec3(0.0,-(1.0/8.0),0.0);
    }
}

// TODO: Rewrite this to be more accurate
uint8_t getFluidMetadata(uint8_t& blockType, uint8_t& blockMetaData, int& x, uint& y, int& z, World* world, glm::vec3 vertPos) {
    Block* xPlus = world->getBlock(x + 1, y, z);
    Block* xMinus = world->getBlock(x - 1, y, z);
    Block* zPlus = world->getBlock(x, y, z + 1);
    Block* zMinus = world->getBlock(x, y, z - 1);
    Block* xPlusZPlus = world->getBlock(x + 1, y, z + 1);
    Block* xPlusZMinus = world->getBlock(x + 1, y, z - 1);
    Block* xMinusZPlus = world->getBlock(x - 1, y, z + 1);
    Block* xMinusZMinus = world->getBlock(x - 1, y, z - 1);
    uint8_t metadata = blockMetaData;

    // Check neighboring blocks along the X and Z axes
    if (xPlus && xPlus->getBlockType() == blockType && vertPos.x > 0.0) {
        if (xPlus->getMetaData() < blockMetaData) {
            metadata = xPlus->getMetaData();
        } else if (xPlus->getMetaData() & 0x8) {
            metadata = 8;
        }
    }
    if (xMinus && xMinus->getBlockType() == blockType && vertPos.x < 0.0) {
        if (xMinus->getMetaData() < blockMetaData) {
            metadata = xMinus->getMetaData();
        } else if (xMinus->getMetaData() & 0x8) {
            metadata = 8;
        }
    }
    if (zPlus && zPlus->getBlockType() == blockType && vertPos.z > 0.0) {
        if (zPlus->getMetaData() < blockMetaData) {
            metadata = zPlus->getMetaData();
        } else if (zPlus->getMetaData() & 0x8) {
            metadata = 8;
        }
    }
    if (zMinus && zMinus->getBlockType() == blockType && vertPos.z < 0.0) {
        if (zMinus->getMetaData() < blockMetaData) {
            metadata = zMinus->getMetaData();
        } else if (zMinus->getMetaData() & 0x8) {
            metadata = 8;
        }
    }

    // Check diagonal neighbors for lower metadata
    if (xPlusZPlus && xPlusZPlus->getBlockType() == blockType && vertPos.x > 0.0 && vertPos.z > 0.0) {
        if (xPlusZPlus->getMetaData() < blockMetaData) {
            metadata = xPlusZPlus->getMetaData();
        } else if (xPlusZPlus->getMetaData() & 0x8) {
            metadata = 8;
        }
    }
    if (xPlusZMinus && xPlusZMinus->getBlockType() == blockType && vertPos.x > 0.0 && vertPos.z < 0.0) {
        if (xPlusZMinus->getMetaData() < blockMetaData) {
            metadata = xPlusZMinus->getMetaData();
        } else if (xPlusZMinus->getMetaData() & 0x8) {
            metadata = 8;
        }
    }
    if (xMinusZPlus && xMinusZPlus->getBlockType() == blockType && vertPos.x < 0.0 && vertPos.z > 0.0) {
        if (xMinusZPlus->getMetaData() < blockMetaData) {
            metadata = xMinusZPlus->getMetaData();
        } else if (xMinusZPlus->getMetaData() & 0x8) {
            metadata = 8;
        }
    }
    if (xMinusZMinus && xMinusZMinus->getBlockType() == blockType && vertPos.x < 0.0 && vertPos.z < 0.0) {
        if (xMinusZMinus->getMetaData() < blockMetaData) {
            metadata = xMinusZMinus->getMetaData();
        } else if (xMinusZMinus->getMetaData() & 0x8) {
            metadata = 8;
        }
    }
    return metadata;
}

DummyMesh ChunkBuilder::buildChunk(Chunk* chunk, bool smoothLighting, uint8_t maxSkyLight) {
    std::vector<Vertex> worldVertices;
    std::vector<GLuint> worldIndices;

    std::vector<Vertex> waterVertices;
    std::vector<GLuint> waterIndices;

    int chunkX = chunk->x*16;
    int chunkZ = chunk->z*16;

    //std::cout << "Chunk" << " " << chunk->x << ", " << chunk->z << std::endl;
    for (uint y = 0; y < 128; y++) {
        for (int z = 0; z < 16; z++) {
            for (int x = 0; x < 16; x++) {
                // Get next block to process
                Block* b = world->getBlock(x+chunkX,y,z+chunkZ);
                // Check if the block is air
                if (!b || b == nullptr) {
                    continue;
                }
                unsigned char blockType = b->getBlockType();
                if (blockType == 0) {
                    continue;
                }
                // If the block is fully surrounded, don't bother loading it
                /*if (isSurrounded(x,y,z,blockType)) {
                    continue;
                }*/
                unsigned char blockMetaData = b->getMetaData();

                // Figure out the blocks coordinates in the world
                //glm::vec3 pos = glm::vec3(float(x), float(y), float(z));

                if (isCube(blockType) || isFluid(blockType)) {
                    glm::vec3 defaultColor = glm::vec3(1.0,1.0,1.0);
                    glm::vec3 normal = glm::vec3(0.0,1.0,0.0);
                    glm::vec3 startPos = glm::vec3(x+chunkX,y,z+chunkZ);
                    /*if (isFluid(blockType)) {
                        defaultColor = glm::vec3(0.1,0.2,0.9);
                    }
                    if (isBiomeColored(blockType)) {
                        defaultColor = getBiomeBlockColor(blockType,blockMetaData,normal);
                    }*/
                    Block* xNeighbor = nullptr;

                    // Greedy Meshing
                    bool differentBlock = false;
                    float blockLength = 0.0;
                    int grownBy = 1;
                    while(!differentBlock) {
                        xNeighbor = world->getBlock(x+grownBy,y,z);
                        if (xNeighbor && xNeighbor->getBlockType() == blockType && (x+grownBy < 16) && !isSurrounded(x+grownBy,y,z,blockType)) {
                            grownBy++;
                            blockLength += 0.0625;
                        } else {
                            differentBlock = true;
                        }
                    }
                    glm::vec3 dimensions = glm::vec3(grownBy,0,1);

                    // Creating Quad
                    Vertex startVertex1(
                        startPos,
                        normal,
                        defaultColor,
                        glm::vec2(0.0,1.0)
                    );
                    glm::vec3 position2 = startPos;
                    position2.z += dimensions.z;
                    Vertex startVertex2(
                        position2,
                        normal,
                        defaultColor,
                        glm::vec2(0.0625,1.0)
                    );
                    glm::vec3 position3 = startPos;
                    position3.x += dimensions.x;

                    Vertex endVertex1(
                        position3,
                        normal,
                        defaultColor,
                        glm::vec2(0.0,0.9375)
                    );

                    Vertex endVertex2(
                        startPos+dimensions,
                        normal,
                        defaultColor,
                        glm::vec2(0.0625,0.9375)
                    );

                    worldVertices.push_back(startVertex1);
                    worldVertices.push_back(endVertex2);
                    worldVertices.push_back(endVertex1);
                    worldVertices.push_back(startVertex1);
                    worldVertices.push_back(startVertex2);
                    worldVertices.push_back(endVertex2);
                    GLuint totalWorldIndices = worldIndices.size();
                    for (uint i = 0; i < 6; i++) {
                        GLuint newInd = totalWorldIndices + i;
                        worldIndices.push_back(newInd);
                    }
                    x+=grownBy;
                }/* else {
                    cachedMesh = getBlockMesh(blockType,x,y,z,blockMetaData);
                    Mesh* mesh = cachedMesh;
                    if (!mesh) {
                        continue;
                    }
                    for (uint v = 0; v < mesh->vertices.size(); v++) {
                        glm::vec3 offset = glm::vec3(0.0f);
                        glm::vec3 normal = glm::vec3(mesh->vertices[v].normal);
                        if (isHidden(world, x, y, z, b, normal)) {
                            continue;
                        }
                        glm::vec3 color = getBiomeBlockColor(blockType, blockMetaData, &mesh->vertices[v]);

                        // Fluid height adjustment
                        glm::vec3 vertPos = mesh->vertices[v].position;
                        if (isFluid(blockType) && vertPos.y > 0.25f) {
                            Block* aboveBlock = world->getBlock(x, y + 1, z);
                            if (aboveBlock && aboveBlock->getBlockType() != blockType) {
                                uint8_t metadata = getFluidMetadata(blockType, blockMetaData, x, y, z, world, vertPos);
                                offset = getFluidVertexOffset(metadata);
                            }
                        }
                        vertPos += offset;

                        rotateBlockAccordingToMetaData(vertPos,normal,blockType,blockMetaData);

                        glm::vec3 worldPos = vertPos + pos + 0.5f;
                        glm::vec2 finalUV = mesh->vertices[v].textureUV;
                        // Only affects the side
                        if (normal.y == 0) {
                            finalUV.y = finalUV.y + (offset.y/16);
                        }
                        
                        // Water in it's own thing
                        if (blockType == WATER || blockType == ICE) {
                            color *= getLighting(world,x,y,z,normal,maxSkyLight);
                            waterVertices.push_back(
                                Vertex(
                                    worldPos,
                                    normal,
                                    color,
                                    finalUV
                                )
                            );
                        } else {
                            // Apply light if the block isn't a lightsource
                            if (!isLightSource(blockType)) {
                                if (smoothLighting) {
                                    //color *= getSmoothLighting(world,x,y,z,vertPos,normal,maxSkyLight);
                                    if (!isTransparent(blockType) && blockType != SNOW_LAYER) {
                                        color *= getAmbientOcclusion(world,worldPos,vertPos,normal);
                                    }
                                } else {
                                    color *= getLighting(world,x,y,z,normal,maxSkyLight);
                                }
                            }
                            worldVertices.push_back(
                                Vertex(
                                    worldPos,
                                    normal,
                                    color,
                                    finalUV
                                )
                            );
                        }
                    }
                    GLuint totalWaterIndices = waterIndices.size();
                    GLuint totalWorldIndices = worldIndices.size();

                    if (isFluid(blockType) || blockType == ICE) {
                        for (uint i = 0; i < mesh->indices.size(); i++) {
                            GLuint newInd = totalWaterIndices + mesh->indices[i];
                            waterIndices.push_back(newInd);
                        }
                    } else {
                        for (uint i = 0; i < mesh->indices.size(); i++) {
                            GLuint newInd = totalWorldIndices + mesh->indices[i];
                            worldIndices.push_back(newInd);
                        }
                    }
                }*/
            }
        }
    }

    return DummyMesh(chunk, std::to_string(chunkX) + "_" + std::to_string(chunkZ),
        worldVertices, worldIndices, waterVertices, waterIndices);
}