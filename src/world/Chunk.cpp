#include "world/Chunk.hpp"

Chunk::Chunk() {
    blocks.fill(BlockType::Air);
    dirty = true;
}

BlockType Chunk::get(int x, int y, int z) const {
    if (!inBounds(x, y, z)) {
        return BlockType::Air;
    }

    return blocks[index(x, y, z)];
}

void Chunk::set(int x, int y, int z, BlockType type) {
    if (!inBounds(x, y, z)) {
        return;
    }

    const int i = index(x, y, z);

    if (blocks[i] == type) {
        return;
    }

    blocks[i] = type;
    dirty = true;
}

void Chunk::generateFlatTerrain() {
    blocks.fill(BlockType::Air);

    for (int x = 0; x < SizeX; ++x) {
        for (int z = 0; z < SizeZ; ++z) {
            set(x, 0, z, BlockType::Stone);
            set(x, 1, z, BlockType::Dirt);
            set(x, 2, z, BlockType::Grass);
        }
    }

    dirty = true;
}

bool Chunk::isDirty() const {
    return dirty;
}

void Chunk::markDirty() {
    dirty = true;
}

void Chunk::clearDirty() {
    dirty = false;
}