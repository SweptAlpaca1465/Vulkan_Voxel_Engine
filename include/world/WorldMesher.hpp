#pragma once

#include "renderer/Frustum.hpp"
#include "world/ChunkMesh.hpp"
#include "world/World.hpp"
#include "world/ChunkCoord.hpp"

#include <cstdint>
#include <vector>

struct ChunkRenderSection {
    ChunkCoord coord;
    AABB bounds;
    uint32_t firstIndex = 0;
    uint32_t indexCount = 0;

    bool empty() const {
        return indexCount == 0;
    }
};

struct WorldRenderData {
    ChunkMesh mesh;
    std::vector<ChunkRenderSection> sections;
};

namespace WorldMesher {
    WorldRenderData buildRenderData(const World& world);
}