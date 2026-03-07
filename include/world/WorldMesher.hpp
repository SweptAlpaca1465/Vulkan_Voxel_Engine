#pragma once

#include "world/ChunkMesh.hpp"
#include "world/World.hpp"

namespace WorldMesher {
    ChunkMesh buildCombinedMesh(const World& world);
}