#pragma once

#include "renderer/Frustum.hpp"
#include "world/ChunkCoord.hpp"
#include "world/ChunkMesh.hpp"
#include "world/World.hpp"

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

struct ChunkMeshCacheEntry {
    ChunkCoord coord;
    ChunkMesh mesh;
};

class WorldMesherCache {
public:
    void clear();
    void syncWithWorld(const World& world);
    bool remeshDirtyChunks(World& world);
    WorldRenderData buildRenderData() const;

private:
    std::vector<ChunkMeshCacheEntry> chunkMeshes;

    ChunkMeshCacheEntry* findEntry(int x, int z);
    const ChunkMeshCacheEntry* findEntry(int x, int z) const;
};