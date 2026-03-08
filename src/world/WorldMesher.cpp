#include "world/WorldMesher.hpp"
#include "world/ChunkMesher.hpp"

namespace {
AABB makeChunkBounds(const ChunkCoord& coord) {
    const float minX = static_cast<float>(coord.x * Chunk::SizeX);
    const float minY = 0.0f;
    const float minZ = static_cast<float>(coord.z * Chunk::SizeZ);

    const float maxX = minX + static_cast<float>(Chunk::SizeX);
    const float maxY = minY + static_cast<float>(Chunk::SizeY);
    const float maxZ = minZ + static_cast<float>(Chunk::SizeZ);

    return {
        { minX, minY, minZ },
        { maxX, maxY, maxZ }
    };
}
}

void WorldMesherCache::clear() {
    chunkMeshes.clear();
}

ChunkMeshCacheEntry* WorldMesherCache::findEntry(int x, int z) {
    for (ChunkMeshCacheEntry& entry : chunkMeshes) {
        if (entry.coord.x == x && entry.coord.z == z) {
            return &entry;
        }
    }

    return nullptr;
}

const ChunkMeshCacheEntry* WorldMesherCache::findEntry(int x, int z) const {
    for (const ChunkMeshCacheEntry& entry : chunkMeshes) {
        if (entry.coord.x == x && entry.coord.z == z) {
            return &entry;
        }
    }

    return nullptr;
}

void WorldMesherCache::syncWithWorld(const World& world) {
    std::vector<ChunkMeshCacheEntry> kept;
    kept.reserve(world.getChunks().size());

    for (const WorldChunk& worldChunk : world.getChunks()) {
        if (const ChunkMeshCacheEntry* existing = findEntry(worldChunk.coord.x, worldChunk.coord.z)) {
            kept.push_back(*existing);
        } else {
            ChunkMeshCacheEntry entry;
            entry.coord = worldChunk.coord;
            kept.push_back(entry);
        }
    }

    chunkMeshes = std::move(kept);
}

bool WorldMesherCache::remeshDirtyChunks(World& world) {
    bool anyRemeshed = false;

    syncWithWorld(world);

    for (WorldChunk& worldChunk : world.getChunksMutable()) {
        if (!worldChunk.chunk.isDirty()) {
            continue;
        }

        const WorldChunk* west  = world.findChunk(worldChunk.coord.x - 1, worldChunk.coord.z);
        const WorldChunk* east  = world.findChunk(worldChunk.coord.x + 1, worldChunk.coord.z);
        const WorldChunk* north = world.findChunk(worldChunk.coord.x, worldChunk.coord.z - 1);
        const WorldChunk* south = world.findChunk(worldChunk.coord.x, worldChunk.coord.z + 1);

        ChunkMeshCacheEntry* cacheEntry = findEntry(worldChunk.coord.x, worldChunk.coord.z);
        if (!cacheEntry) {
            continue;
        }

        cacheEntry->mesh = ChunkMesher::build(
            worldChunk.chunk,
            west  ? &west->chunk  : nullptr,
            east  ? &east->chunk  : nullptr,
            north ? &north->chunk : nullptr,
            south ? &south->chunk : nullptr
        );

        worldChunk.chunk.clearDirty();
        anyRemeshed = true;
    }

    return anyRemeshed;
}

WorldRenderData WorldMesherCache::buildRenderData() const {
    WorldRenderData out;

    for (const ChunkMeshCacheEntry& entry : chunkMeshes) {
        ChunkRenderSection section;
        section.coord = entry.coord;
        section.bounds = makeChunkBounds(entry.coord);
        section.firstIndex = static_cast<uint32_t>(out.mesh.indices.size());
        section.indexCount = static_cast<uint32_t>(entry.mesh.indices.size());

        const float worldOffsetX = static_cast<float>(entry.coord.x * Chunk::SizeX);
        const float worldOffsetZ = static_cast<float>(entry.coord.z * Chunk::SizeZ);

        const uint32_t vertexOffset = static_cast<uint32_t>(out.mesh.vertices.size());

        for (Vertex vertex : entry.mesh.vertices) {
            vertex.position[0] += worldOffsetX;
            vertex.position[2] += worldOffsetZ;
            out.mesh.vertices.push_back(vertex);
        }

        for (uint32_t index : entry.mesh.indices) {
            out.mesh.indices.push_back(index + vertexOffset);
        }

        out.sections.push_back(section);
    }

    return out;
}