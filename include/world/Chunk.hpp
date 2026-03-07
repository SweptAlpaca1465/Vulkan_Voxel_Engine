#pragma once

#include "world/Block.hpp"

#include <array>

class Chunk {
public:
    static constexpr int SizeX = 16;
    static constexpr int SizeY = 16;
    static constexpr int SizeZ = 16;
    static constexpr int BlockCount = SizeX * SizeY * SizeZ;

    Chunk();

    BlockType get(int x, int y, int z) const;
    void set(int x, int y, int z, BlockType type);

    void generateFlatTerrain();

private:
    std::array<BlockType, BlockCount> blocks;

    static int index(int x, int y, int z) {
        return x + SizeX * (z + SizeZ * y);
    }

    static bool inBounds(int x, int y, int z) {
        return x >= 0 && x < SizeX &&
               y >= 0 && y < SizeY &&
               z >= 0 && z < SizeZ;
    }
};