#include "world/Chunk.hpp"

Chunk::Chunk() {
    blocks.fill(BlockType::Air);
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

    blocks[index(x, y, z)] = type;
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
}