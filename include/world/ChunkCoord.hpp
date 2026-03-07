#pragma once

struct ChunkCoord {
    int x = 0;
    int z = 0;

    bool operator==(const ChunkCoord& other) const {
        return x == other.x && z == other.z;
    }
};