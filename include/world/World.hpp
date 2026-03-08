#pragma once

#include "world/Chunk.hpp"
#include "world/ChunkCoord.hpp"

#include <vector>

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
    const WorldChunk* findChunk(int x, int z) const;

private:
    std::vector<WorldChunk> chunks;
};