#pragma once
#include <vector>
#include <algorithm>
#include "chunk.h"
#include "block.h"
#include <cmath>
#include "regionLoader.h"
#include <unordered_set>
#include <mutex>
#include <unordered_map>
#include <shared_mutex>

// Hash function for std::pair<int, int>
struct pair_hash {
    template <class T1, class T2>
    std::size_t operator()(const std::pair<T1, T2>& pair) const {
        return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
    }
};

class World {
    private:
        std::unordered_map<std::pair<int, int>, Chunk*, pair_hash> chunks;
        std::shared_mutex chunk_mutex;
    public:
        RegionLoader* rl = nullptr;
        Chunk* cachedChunk = nullptr;
        Block* cachedBlock = nullptr;
        int cachedBlockX;
        int cachedBlockY;
        int cachedBlockZ;
        std::string name;
        World(const std::string& pName = "");
        void LoadWorld(const std::string& pName);
        // Also add a destructor if you haven't already
        ~World() {
            if (rl != nullptr) {
                delete rl;
                rl = nullptr;
            }
        }

        Chunk* findChunk(int x, int z);
        Chunk* loadChunk(int x, int z, bool nether = false);
        Chunk* getChunk(int x, int z);
        Block* getBlock(int x, int y, int z);
        void addChunk(Chunk* chunk);
        void removeChunk(int x, int z);
        void clearChunks();
        size_t getNumberOfChunks();
        void getChunksInRadius(int x, int z, int radius, std::vector<Chunk*>& newChunks, std::mutex& chunkRadiusMutex, bool nether = false);
};