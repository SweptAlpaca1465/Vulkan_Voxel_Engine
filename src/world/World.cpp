#include "world/World.hpp"

void World::clear() {
    chunks.clear();
}

void World::generateFlatWorld(int radius) {
    chunks.clear();

    for (int cz = -radius; cz <= radius; ++cz) {
        for (int cx = -radius; cx <= radius; ++cx) {
            generateChunk(cx, cz);
        }
    }
}

void World::generateChunk(int x, int z) {
    if (hasChunk(x, z)) {
        return;
    }

    WorldChunk worldChunk;
    worldChunk.coord = { x, z };
    worldChunk.chunk.generateFlatTerrain();
    chunks.push_back(worldChunk);
}

bool World::hasChunk(int x, int z) const {
    return findChunk(x, z) != nullptr;
}

bool World::removeChunk(int x, int z) {
    for (auto it = chunks.begin(); it != chunks.end(); ++it) {
        if (it->coord.x == x && it->coord.z == z) {
            chunks.erase(it);
            return true;
        }
    }

    return false;
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