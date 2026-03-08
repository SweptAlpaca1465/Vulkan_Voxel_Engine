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
    worldChunk.chunk.markDirty();
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

std::vector<WorldChunk>& World::getChunksMutable() {
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

WorldChunk* World::findChunkMutable(int x, int z) {
    for (WorldChunk& chunk : chunks) {
        if (chunk.coord.x == x && chunk.coord.z == z) {
            return &chunk;
        }
    }

    return nullptr;
}

void World::markChunkDirty(int x, int z) {
    WorldChunk* chunk = findChunkMutable(x, z);
    if (chunk) {
        chunk->chunk.markDirty();
    }
}

bool World::hasDirtyChunks() const {
    for (const WorldChunk& worldChunk : chunks) {
        if (worldChunk.chunk.isDirty()) {
            return true;
        }
    }

    return false;
}

bool World::setBlock(int chunkX, int chunkZ, int localX, int y, int localZ, BlockType type) {
    WorldChunk* chunk = findChunkMutable(chunkX, chunkZ);
    if (!chunk) {
        return false;
    }

    chunk->chunk.set(localX, y, localZ, type);

    if (localX == 0) {
        markChunkDirty(chunkX - 1, chunkZ);
    }
    if (localX == Chunk::SizeX - 1) {
        markChunkDirty(chunkX + 1, chunkZ);
    }
    if (localZ == 0) {
        markChunkDirty(chunkX, chunkZ - 1);
    }
    if (localZ == Chunk::SizeZ - 1) {
        markChunkDirty(chunkX, chunkZ + 1);
    }

    return true;
}

namespace {
int floorDiv(int value, int size) {
    return static_cast<int>(std::floor(static_cast<float>(value) / static_cast<float>(size)));
}

int positiveMod(int value, int size) {
    int result = value % size;
    if (result < 0) {
        result += size;
    }
    return result;
}
}

bool World::getBlockGlobal(int worldX, int y, int worldZ, BlockType& outBlock) const {
    if (y < 0 || y >= Chunk::SizeY) {
        outBlock = BlockType::Air;
        return false;
    }

    const int chunkX = floorDiv(worldX, Chunk::SizeX);
    const int chunkZ = floorDiv(worldZ, Chunk::SizeZ);

    const int localX = positiveMod(worldX, Chunk::SizeX);
    const int localZ = positiveMod(worldZ, Chunk::SizeZ);

    const WorldChunk* chunk = findChunk(chunkX, chunkZ);
    if (!chunk) {
        outBlock = BlockType::Air;
        return false;
    }

    outBlock = chunk->chunk.get(localX, y, localZ);
    return true;
}

bool World::setBlockGlobal(int worldX, int y, int worldZ, BlockType type) {
    if (y < 0 || y >= Chunk::SizeY) {
        return false;
    }

    const int chunkX = floorDiv(worldX, Chunk::SizeX);
    const int chunkZ = floorDiv(worldZ, Chunk::SizeZ);

    const int localX = positiveMod(worldX, Chunk::SizeX);
    const int localZ = positiveMod(worldZ, Chunk::SizeZ);

    return setBlock(chunkX, chunkZ, localX, y, localZ, type);
}