#include "world/WorldMesher.hpp"
#include "world/ChunkMesher.hpp"

namespace WorldMesher {

ChunkMesh buildCombinedMesh(const World& world) {
    ChunkMesh combined;

    for (const WorldChunk& worldChunk : world.getChunks()) {
        const WorldChunk* west  = world.findChunk(worldChunk.coord.x - 1, worldChunk.coord.z);
        const WorldChunk* east  = world.findChunk(worldChunk.coord.x + 1, worldChunk.coord.z);
        const WorldChunk* north = world.findChunk(worldChunk.coord.x, worldChunk.coord.z - 1);
        const WorldChunk* south = world.findChunk(worldChunk.coord.x, worldChunk.coord.z + 1);

        ChunkMesh localMesh = ChunkMesher::build(
            worldChunk.chunk,
            west  ? &west->chunk  : nullptr,
            east  ? &east->chunk  : nullptr,
            north ? &north->chunk : nullptr,
            south ? &south->chunk : nullptr
        );

        const float worldOffsetX = static_cast<float>(worldChunk.coord.x * Chunk::SizeX);
        const float worldOffsetZ = static_cast<float>(worldChunk.coord.z * Chunk::SizeZ);

        const uint32_t indexOffset = static_cast<uint32_t>(combined.vertices.size());

        for (Vertex vertex : localMesh.vertices) {
            vertex.position[0] += worldOffsetX;
            vertex.position[2] += worldOffsetZ;
            combined.vertices.push_back(vertex);
        }

        for (uint32_t index : localMesh.indices) {
            combined.indices.push_back(index + indexOffset);
        }
    }

    return combined;
}

}