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
    void generateFlatWorld(int radius);

    const std::vector<WorldChunk>& getChunks() const;
    const WorldChunk* findChunk(int x, int z) const;

private:
    std::vector<WorldChunk> chunks;
};