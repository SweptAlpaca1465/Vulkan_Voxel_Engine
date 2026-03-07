#include "world/World.hpp"

void World::generateFlatWorld(int radius) {
    chunks.clear();

    for (int cz = -radius; cz <= radius; ++cz) {
        for (int cx = -radius; cx <= radius; ++cx) {
            WorldChunk worldChunk;
            worldChunk.coord = {cx, cz};
            worldChunk.chunk.generateFlatTerrain();
            chunks.push_back(worldChunk);
        }
    }
}

const std::vector<WorldChunk>& World::getChunks() const {
    return chunks;
}

const WorldChunk* World::findChunk(int x, int z) const {
    for (const WorldChunk& chunk : chunks) {
        if (chunk.coord.x == x && chunk.coord.z == z) {
            return &chunk;
        }
    }
    return nullptr;
}