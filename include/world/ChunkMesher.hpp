#pragma once

#include "world/Chunk.hpp"
#include "world/ChunkMesh.hpp"

namespace ChunkMesher {
    ChunkMesh build(
        const Chunk& chunk,
        const Chunk* west,
        const Chunk* east,
        const Chunk* north,
        const Chunk* south
    );
}