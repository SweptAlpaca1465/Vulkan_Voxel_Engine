#pragma once

#include "world/Block.hpp"
#include "world/Chunk.hpp"
#include "world/ChunkCoord.hpp"

#include <vector>
#include <cmath>

struct WorldChunk {
    ChunkCoord coord;
    Chunk chunk;
};

class World {
public:
    void clear();
    void generateFlatWorld(int radius);
    void generateChunk(int x, int z);

    bool hasChunk(int x, int z) const;
    bool removeChunk(int x, int z);

    const std::vector<WorldChunk>& getChunks() const;
    std::vector<WorldChunk>& getChunksMutable();

    const WorldChunk* findChunk(int x, int z) const;
    WorldChunk* findChunkMutable(int x, int z);

    void markChunkDirty(int x, int z);
    bool hasDirtyChunks() const;

    bool setBlock(int chunkX, int chunkZ, int localX, int y, int localZ, BlockType type);

    bool getBlockGlobal(int worldX, int y, int worldZ, BlockType& outBlock) const;
    bool setBlockGlobal(int worldX, int y, int worldZ, BlockType type);

private:
    std::vector<WorldChunk> chunks;
};