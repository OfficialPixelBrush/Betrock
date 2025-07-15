#include "chunkBuilder.h"
#include <stdexcept>  // For std::runtime_error

#include <stdexcept>  // For std::runtime_error

const float lightArray[16] = {0.035f, 0.044f, 0.055f, 0.069f, 0.086f, 0.107f, 0.134f, 0.168f, 0.21f, 0.262f, 0.328f, 0.41f, 0.512f, 0.64f, 0.8f, 1.0f};

struct Light {
    float blockLight = 1.0;
    float skyLight = 1.0;
};

typedef struct Light Light;

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

// ---------------------------------------------------------------------------
//  Beta 1.7.3‑style Perlin noise (temperature / rainfall) – public‑domain code
// ---------------------------------------------------------------------------
#include <array>
#include <cstdint>
#include <random>
#include <vector>
#include <cmath>

class Beta173Perlin {
public:
    explicit Beta173Perlin(std::uint64_t worldSeed, int octaves = 6)
        : m_octaves(octaves),
          m_perm(512)                                // 2 × 256 for fast wrap
    {
        /*
         * Minecraft Beta calls Java’s java.util.Random with the world seed,
         * then initialises a 256‑entry permutation table using a Fisher‑Yates
         * shuffle.  Java Random is a 48‑bit LCG:  (seed * 0x5DEECE66D + 11) & 0xFFFF_FFFFFFFF.
         */
        auto nextInt = [seed = (worldSeed ^ 0x5DEECE66DL) & ((1LL << 48) - 1)]() mutable {
            seed = (seed * 0x5DEECE66DL + 0xB) & ((1LL << 48) - 1);
            return static_cast<int>(seed >> 17);
        };

        std::array<int, 256> p{};
        for (int i = 0; i < 256; ++i) p[i] = i;

        for (int i = 255; i >= 0; --i) {
            int j = nextInt() % (i + 1);
            std::swap(p[i], p[j]);
            m_perm[i] = m_perm[i + 256] = p[i];
        }
    }

    // 2‑D noise sample; identical to Beta 1.7.3
    double noise(double x, double z) const {
        const int X = fastFloor(x) & 255;
        const int Z = fastFloor(z) & 255;
        x -= fastFloor(x);
        z -= fastFloor(z);
        const double u = fade(x);
        const double v = fade(z);

        const int A  = m_perm[X] + Z;
        const int B  = m_perm[X + 1] + Z;

        return lerp(v,
                    lerp(u, grad(m_perm[A  ], x    , z    ),
                             grad(m_perm[B  ], x - 1, z    )),
                    lerp(u, grad(m_perm[A+1], x    , z - 1),
                             grad(m_perm[B+1], x - 1, z - 1)));
    }

    // Full FBM stack (6 octaves, identical gain/frequency)
    double fbm(double x, double z,
               double baseFreq  = 0.05,   // 1/20 blocks – official value
               double lacunarity = 2.0,
               double gain       = 0.5) const
    {
        double amp = 1.0, freq = baseFreq, sum = 0.0;
        for (int i = 0; i < m_octaves; ++i) {
            sum += amp * noise(x * freq, z * freq);
            freq *= lacunarity;
            amp  *= gain;
        }
        // Java Perlin returns in [‑1,1]; Beta rescales to [0,1]
        return 0.5 * (sum + 1.0);
    }

private:
    static int fastFloor(double x)        { return x < 0 ? int(x) - 1 : int(x); }
    static double fade(double t)          { return t * t * t * (t * (t * 6 - 15) + 10); }
    static double lerp(double a, double b, double t) { return a + t * (b - a); }
    static double grad(int h, double x, double z) {
        switch (h & 3) {                 // 2‑D gradient hash
            case 0: return  x + z;
            case 1: return -x + z;
            case 2: return  x - z;
            default:return -x - z;
        }
    }

    int m_octaves;
    std::vector<int> m_perm;
};

// ---------------------------------------------------------------------------
//  Example – reproducing the Beta foliage colour lookup at (x,z) world coords
// ---------------------------------------------------------------------------
inline std::uint32_t beta173GrassColour(std::int64_t worldSeed, int x, int z)
{
    static constexpr std::uint32_t biomeLUT[64] = {
        /* 4×4 Whittaker‑diagram in Beta – packed RGB ints (ABGR) */
        0xFF007F0E,0xFF169F0D,0xFF1FAD10,0xFF1FA514,
        0xFF3BA317,0xFF4FAB21,0xFF5FB32A,0xFF68B330,
        0xFF5AAC1C,0xFF6CBC24,0xFF7EC32D,0xFF88C633,
        0xFF86BA24,0xFF91C128,0xFFA2CC2E,0xFFAED334
    };

    Beta173Perlin perlin(worldSeed);

    double temp    = perlin.fbm(double(x), double(z),
                                0.025);               // temperature scale
    double rain    = perlin.fbm(double(x), double(z),
                                0.05);                // rainfall scale
    double moist   = rain * temp;                     // Beta’s quirk

    // Map to 4×4 grid the same way the original does
    int tt = int((1.0 - temp ) * 4.0);                // invert T axis
    int rr = int((1.0 - moist) * 4.0);
    tt = std::clamp(tt, 0, 3);
    rr = std::clamp(rr, 0, 3);
    return biomeLUT[rr * 4 + tt];
}

void printColorBlock(int r, int g, int b) {
    // Ensure values are within 0–255
    r = std::max(0, std::min(255, r));
    g = std::max(0, std::min(255, g));
    b = std::max(0, std::min(255, b));

    std::ostringstream oss;
    // ANSI escape: \x1b[48;2;<r>;<g>;<b>m sets background color
    oss << "\x1b[48;2;" << r << ";" << g << ";" << b << "m";
    // Print a block character
    oss << "  ";
    // Reset terminal colors
    oss << "\x1b[0m";

    std::cout << oss.str();
}

glm::vec3 getBiomeBlockColor(unsigned char blockType, unsigned char blockMetaData, Vertex* vert, std::int64_t worldSeed, int x, int z) {
    uint32_t col = beta173GrassColour(worldSeed, x, z);
    float blue  = float((col      ) & 0xFF)/255.0;       // B
    float green = float((col >> 8 ) & 0xFF)/255.0;       // G
    float red   = float((col >> 16) & 0xFF)/255.0;       // R
    glm::vec3 color = glm::vec3(red, green, blue);
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

Light getLightingOfCurrent(World* world, int x, int y, int z) {
    // Array for light values

    try {

        // Get the adjacent block
        Block* currentBlock = world->getBlock(x, y, z);
        if (currentBlock == nullptr) {
            throw std::runtime_error("Current block pointer is null.");
        }

        // Get lighting values from the adjacent block
        return Light{
            lightArray[currentBlock->lightLevel],
            lightArray[currentBlock->skyLightLevel]
        };

    } catch (const std::exception& e) {
        return Light { 1.0f, 1.0f };  // Return a default value in case of an error
    }
}

Light getLighting(World* world, int x, int y, int z, glm::vec3 normal) {
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
        return Light{
            lightArray[adjacentBlock->lightLevel],
            lightArray[adjacentBlock->skyLightLevel]
        };

    } catch (const std::exception& e) {
        return Light{1.0f,1.0f};  // Return a default value in case of an error
    }
}

bool isVisible(World* world, int x, int y, int z, uint8_t& cbType, uint8_t& cbMeta, glm::vec3 normal) {
    // Calculate adjacent block coordinates
    int adjX = x + static_cast<int>(normal.x);
    int adjY = y + static_cast<int>(normal.y);
    int adjZ = z + static_cast<int>(normal.z);

    // Get the adjacent block
    Block* adjacentBlock = world->getBlock(adjX, adjY, adjZ);
    // No adjacent Block, not visible
    if (!adjacentBlock) {
        return false;
    }
    uint8_t abType = adjacentBlock->blockType;
    uint8_t abMeta = adjacentBlock->metaData;

    // Snow Layer optimization
    if (cbType == SNOW_LAYER && !isTransparent(abType) && normal.y < 0.0) {
        return false;
    }

    if (abType == SNOW_LAYER && !isTransparent(cbType) && normal.y > 0.0) {
        return false;
    }

    // If it's the same as the checking block
    if (isFluid(cbType) && abType == ICE) {
        return false;
    }

    // Prevent top faces from getting culled on Liquids
    if (isFluid(cbType) && cbType != abType && normal.y > 0.0) {
        return true;
    }

    // All fluids are one
    if (isFluid(cbType) && cbType == abType) {
        return false;
    }

    // Billboards are always visible
    if (isBillboard(cbType)) {
        return true;
    }

    /*
    Doesn't work for the top of slabs against full blocks
    if (isPartialBlock(cbType) && isFull(abType)) {
        return false;
    } 
    */

    if (isTransparent(cbType)) {
        if (cbType == abType || isFull(abType)) {
            return false;
        }
    }

    if (isFull(cbType) && isFull(abType)) {
        return false;
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
    if (blockType == AIR) return nullptr;

    std::string specialQuery;

    // Handle Snow on Grass
    if (blockType == GRASS) {
        Block* above = world->getBlock(x, y + 1, z);
        if (above && above->blockType == SNOW_LAYER) {
            specialQuery = "Snow";
        }
    }

    // Normalize metadata for specific blocks
    if (blockType == LEAVES) {
        blockMetaData &= 0x3;
    } else if (blockType == BED || blockType == WOODEN_DOOR || blockType == IRON_DOOR) {
        blockMetaData &= 0x8;
    }

    // Set of block types that care about metadata
    static const std::unordered_set<uint8_t> metadataSensitiveBlocks = {
        LEAVES,
        LOG,
        TALLGRASS,
        TORCH,
        STONE_STAIRS,
        OAK_STAIRS,
        WOOL,
        PISTON,
        TRAPDOOR,
        WOODEN_DOOR,
        IRON_DOOR,
        UNLIT_REDSTONE_TORCH,
        REDSTONE_TORCH,
        BED,
        WOODEN_PRESSURE_PLATE,
        STONE_PRESSURE_PLATE
    };

    // Check cached mesh first
    if (cachedMesh) {
        std::vector<std::string> parts = splitString(cachedMesh->name, '_');
        if (parts.size() >= 2) {
            try {
                if (blockType == std::stoi(parts[0]) &&
                    blockMetaData == std::stoi(parts[1])) {
                    return cachedMesh;
                }
            } catch (...) {
                // Ignore parsing errors
            }
        }
    }

    // Search through available meshes
    for (auto& m : model->meshes) {
        std::vector<std::string> parts = splitString(m->name, '_');
        if (parts.size() < 2) continue;

        try {
            uint8_t meshType = static_cast<uint8_t>(std::stoi(parts[0]));
            uint8_t meshMeta = static_cast<uint8_t>(std::stoi(parts[1]));

            if (blockType != meshType) continue;

            // Metadata-specific comparison
            if (metadataSensitiveBlocks.contains(blockType)) {
                if (blockMetaData != meshMeta) continue;
                return m.get();
            }

            // Special query (e.g., "Snow")
            if (!specialQuery.empty()) {
                if (parts.size() > 3 && specialQuery == parts[3]) {
                    return m.get();
                } else {
                    continue;
                }
            }

            // Default match (type matches, metadata ignored)
            return m.get();

        } catch (...) {
            // Skip mesh if parsing fails
            continue;
        }
    }

    // Fallback mesh (should never be hit in ideal case)
    return model->meshes[0].get();
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

Light getSmoothLighting(World* world, int x, int y, int z, glm::vec3 vertexPosition, glm::vec3 normal) {
    uint8_t blockLight = 0;
    uint8_t skyLight = 0;
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
                uint8_t winningBlockLight = b->lightLevel; //std::max(b->lightLevel, std::min(b->skyLightLevel, maxSkyLight));
                uint8_t winningSkyLight = b->skyLightLevel; //std::max(b->lightLevel, std::min(b->skyLightLevel, maxSkyLight));
                blockLight = std::max(blockLight, winningBlockLight);
                skyLight = std::max(skyLight, winningSkyLight);
            }
        }
    }

    return Light{lightArray[blockLight],lightArray[skyLight]};
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
                    if (!isVisible(world, x, y, z, blockType, blockMetaData, normal)) {
                        continue;
                    }
                    glm::vec3 color = getBiomeBlockColor(blockType, blockMetaData, &mesh->vertices[v], 404, x, z);

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
                    Light l;
                    // Water in it's own thing
                    if (blockType == WATER || blockType == ICE) {
                        l = getLighting(world,x,y,z,normal);
                        waterVertices.push_back(
                            Vertex(
                                worldPos,
                                normal,
                                color,
                                finalUV,
                                l.skyLight,
                                l.blockLight
                            )
                        );
                    } else {
                        // Apply light if the block isn't a lightsource
                        if (!isLightSource(blockType)) {
                            if (isBillboard(blockType)) {
                                l = getLightingOfCurrent(world,x,y,z);
                            } else {
                                if (smoothLighting) {
                                    l = getSmoothLighting(world,x,y,z,vertPos,normal);
                                    if (!isTransparent(blockType) && blockType != SNOW_LAYER) {
                                        color *= getAmbientOcclusion(world,worldPos,vertPos,normal);
                                    }
                                } else {
                                    l = getLighting(world,x,y,z,normal);
                                }
                            }
                        }
                        if (isLightSource(blockType)) l.blockLight++;
                        worldVertices.push_back(
                            Vertex(
                                worldPos,
                                normal,
                                color,
                                finalUV,
                                l.skyLight,
                                l.blockLight
                            )
                        );
                    }
                }

                GLuint totalWaterIndices = waterIndices.size();
                GLuint totalWorldIndices = worldIndices.size();

                if (isFluid(blockType) || blockType == ICE) {
                    for (uint i = 0; i < mesh->indices.size(); i++) {
                        waterIndices.push_back(totalWaterIndices + mesh->indices[i]);
                    }
                } else {
                    for (uint i = 0; i < mesh->indices.size(); i++) {
                        worldIndices.push_back(totalWorldIndices + mesh->indices[i]);
                    }
                }
            }
        }
    }

    return DummyMesh(chunk, std::to_string(chunkX) + "_" + std::to_string(chunkZ),
        worldVertices, worldIndices, waterVertices, waterIndices);
}