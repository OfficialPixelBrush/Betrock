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
            blocks[i] = blockPointers[i]->blockType;
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

glm::vec3 getBiomeBlockColor(unsigned char blockType, unsigned char blockMetaData, Vertex* vert) {
    glm::vec3 color = glm::vec3(0.57, 0.73, 0.34);
    // Biome Colored
    if ((blockType == GRASS && vert->normal.y > 0.0f) || (blockType == TALLGRASS)) {
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
}

float getLightingOfCurrent(World* world, int x, int y, int z, uint8_t maxSkyLight) {
    // Array for light values

    try {

        // Get the adjacent block
        Block* currentBlock = world->getBlock(x, y, z);
        if (currentBlock == nullptr) {
            throw std::runtime_error("Current block pointer is null.");
        }

        // Get lighting values from the adjacent block
        int light = std::max(currentBlock->lightLevel, std::min(currentBlock->skyLightLevel, maxSkyLight));

        return lightArray[light];

    } catch (const std::exception& e) {
        return 1.0f;  // Return a default value in case of an error
    }
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
        int light = std::max(adjacentBlock->lightLevel, std::min(adjacentBlock->skyLightLevel, maxSkyLight));

        return lightArray[light];

    } catch (const std::exception& e) {
        return 1.0f;  // Return a default value in case of an error
    }
}

bool isHidden(World* world, int x, int y, int z, uint8_t& cbType, uint8_t& cbMeta, glm::vec3 normal) {
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
    uint8_t abType = adjacentBlock->blockType;
    uint8_t abMeta = adjacentBlock->metaData;

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

Mesh* ChunkBuilder::getBlockMesh(uint8_t blockType, int x, int y, int z, uint8_t blockMetaData) {
    if (blockType == AIR) {
        return nullptr;
    }
    std::string specialQuery = "";

    // Snow
    if (blockType == GRASS) {
        Block* b = world->getBlock(x,y+1,z);
        if (b && b->blockType == SNOW_LAYER) {
            specialQuery = "Snow";
        }
    }

    // Leaves (removes decay data)
    if (blockType == LEAVES) {
        blockMetaData &= 0x3;
    }

    // Blocks that should ignore rotation data
    if (blockType == BED  || blockType == WOODEN_DOOR || blockType == IRON_DOOR) {
        blockMetaData &= 0x8;
    }

    std::vector<std::string> compareTo;
    // Check Cached Mesh first to save time
    if (cachedMesh != nullptr) {
        compareTo = splitString(cachedMesh->name,'_');
        if (blockType == std::stoi(compareTo[0]) && blockMetaData == std::stoi(compareTo[1])) {
            return cachedMesh;
        }
    }
    
    // Search for the mesh
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
                blockType == BED
            ) {
                if (blockMetaData == std::stoi(compareTo[1])) {
                    return &m;
                } else {
                    continue;
                }
            }
            
            if (specialQuery == "") {
                return &m;
            }
            if (specialQuery == compareTo[3]) {
                return &m;
            }
        }
    }
    return &model->meshes[0];
}

float getAmbientOcclusion(World* world, glm::vec3 position, glm::vec3 vertexPosition, glm::vec3 normal) {
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
        b1Type = b1->blockType;
    }
    b2 = world->getBlock(floor(off2.x), floor(off2.y), floor(off2.z));
    if (b2) {
        b2Type = b2->blockType;
    }
    bc = world->getBlock(floor(offc.x), floor(offc.y), floor(offc.z));
    if (bc) {
        bcType = bc->blockType;
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

int roundDirectional(float value) {
    return (value > 0) ? static_cast<int>(std::ceil(value)) : static_cast<int>(std::floor(value));
}

float getSmoothLighting(World* world, int x, int y, int z, glm::vec3 vertexPosition, glm::vec3 normal, uint8_t maxSkyLight) {
    uint8_t light = 0;
    Block* b = nullptr;
    int xOffset = roundDirectional(vertexPosition.x);
    int yOffset = roundDirectional(vertexPosition.y);
    int zOffset = roundDirectional(vertexPosition.z);

    // Get the adjacent blocks along face
    for (int aOff = 0; aOff <= 1; aOff++) {
        for (int bOff = 0; bOff <= 1; bOff++) {
            if (normal.x > 0.0) {
                b = world->getBlock(x+1, y+aOff*yOffset, z+bOff*zOffset);
            } else if (normal.x < 0.0) {
                b = world->getBlock(x-1, y+aOff*yOffset, z+bOff*zOffset);
            } else if (normal.y > 0.0) {
                b = world->getBlock(x+aOff*xOffset, y+1, z+bOff*zOffset);
            } else if (normal.y < 0.0) {
                b = world->getBlock(x+aOff*xOffset, y-1, z+bOff*zOffset);
            } else if (normal.z > 0.0) {
                b = world->getBlock(x+aOff*xOffset, y+bOff*yOffset, z+1);
            } else if (normal.z < 0.0) {
                b = world->getBlock(x+aOff*xOffset, y+bOff*yOffset, z-1);
            } else {
                b = nullptr;
            }
            if (b) {
                // Air is transparent, so we can ignore it too
                uint8_t winningLight = std::max(b->lightLevel, std::min(b->skyLightLevel, maxSkyLight));
                light = std::max(light, winningLight);
            }
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
    if (xPlus && xPlus->blockType == blockType && vertPos.x > 0.0) {
        if (xPlus->metaData < blockMetaData) {
            metadata = xPlus->metaData;
        } else if (xPlus->metaData & 0x8) {
            metadata = 8;
        }
    }
    if (xMinus && xMinus->blockType == blockType && vertPos.x < 0.0) {
        if (xMinus->metaData < blockMetaData) {
            metadata = xMinus->metaData;
        } else if (xMinus->metaData & 0x8) {
            metadata = 8;
        }
    }
    if (zPlus && zPlus->blockType == blockType && vertPos.z > 0.0) {
        if (zPlus->metaData < blockMetaData) {
            metadata = zPlus->metaData;
        } else if (zPlus->metaData & 0x8) {
            metadata = 8;
        }
    }
    if (zMinus && zMinus->blockType == blockType && vertPos.z < 0.0) {
        if (zMinus->metaData < blockMetaData) {
            metadata = zMinus->metaData;
        } else if (zMinus->metaData & 0x8) {
            metadata = 8;
        }
    }

    // Check diagonal neighbors for lower metadata
    if (xPlusZPlus && xPlusZPlus->blockType == blockType && vertPos.x > 0.0 && vertPos.z > 0.0) {
        if (xPlusZPlus->metaData < blockMetaData) {
            metadata = xPlusZPlus->metaData;
        } else if (xPlusZPlus->metaData & 0x8) {
            metadata = 8;
        }
    }
    if (xPlusZMinus && xPlusZMinus->blockType == blockType && vertPos.x > 0.0 && vertPos.z < 0.0) {
        if (xPlusZMinus->metaData < blockMetaData) {
            metadata = xPlusZMinus->metaData;
        } else if (xPlusZMinus->metaData & 0x8) {
            metadata = 8;
        }
    }
    if (xMinusZPlus && xMinusZPlus->blockType == blockType && vertPos.x < 0.0 && vertPos.z > 0.0) {
        if (xMinusZPlus->metaData < blockMetaData) {
            metadata = xMinusZPlus->metaData;
        } else if (xMinusZPlus->metaData & 0x8) {
            metadata = 8;
        }
    }
    if (xMinusZMinus && xMinusZMinus->blockType == blockType && vertPos.x < 0.0 && vertPos.z < 0.0) {
        if (xMinusZMinus->metaData < blockMetaData) {
            metadata = xMinusZMinus->metaData;
        } else if (xMinusZMinus->metaData & 0x8) {
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
    for (int x = chunkX; x < 16+chunkX; x++) {
        for (int z = chunkZ; z < 16+chunkZ; z++) {
            for (uint y = 0; y < 128; y++) {
                // Get next block to process
                Block* b = world->getBlock(x,y,z);
                // Check if the block is air
                if (!b || b == nullptr) {
                    continue;
                }
                unsigned char blockType = b->blockType;
                if (blockType == 0) {
                    continue;
                }
                // If the block is fully surrounded, don't bother loading it
                if (isSurrounded(x,y,z,blockType)) {
                    continue;
                }
                unsigned char blockMetaData = b->metaData;

                // Figure out the blocks coordinates in the world
                glm::vec3 pos = glm::vec3(float(x), float(y), float(z));

                cachedMesh = getBlockMesh(blockType,x,y,z,blockMetaData);
                Mesh* mesh = cachedMesh;
                if (!mesh) {
                    continue;
                }
                for (uint v = 0; v < mesh->vertices.size(); v++) {
                    glm::vec3 offset = glm::vec3(0.0f);
                    glm::vec3 normal = glm::vec3(mesh->vertices[v].normal);
                    if (isHidden(world, x, y, z, blockType, blockMetaData, normal)) {
                        continue;
                    }
                    glm::vec3 color = getBiomeBlockColor(blockType, blockMetaData, &mesh->vertices[v]);

                    // Fluid height adjustment
                    glm::vec3 vertPos = mesh->vertices[v].position;
                    if (isFluid(blockType) && vertPos.y > 0.25f) {
                        Block* aboveBlock = world->getBlock(x, y + 1, z);
                        if (aboveBlock && aboveBlock->blockType != blockType) {
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
                            if (isBillboard(blockType)) {
                                color*= getLightingOfCurrent(world,x,y,z,maxSkyLight);
                            } else {
                                if (smoothLighting) {
                                    color *= getSmoothLighting(world,x,y,z,vertPos,normal,maxSkyLight);
                                    if (!isTransparent(blockType) && blockType != SNOW_LAYER) {
                                        color *= getAmbientOcclusion(world,worldPos,vertPos,normal);
                                    }
                                } else {
                                    color *= getLighting(world,x,y,z,normal,maxSkyLight);
                                }
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
            }
        }
    }

    return DummyMesh(chunk, std::to_string(chunkX) + "_" + std::to_string(chunkZ),
        worldVertices, worldIndices, waterVertices, waterIndices);
}