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

namespace WorldMesher {

WorldRenderData buildRenderData(const World& world) {
    WorldRenderData out;

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

        ChunkRenderSection section;
        section.coord = worldChunk.coord;
        section.bounds = makeChunkBounds(worldChunk.coord);
        section.firstIndex = static_cast<uint32_t>(out.mesh.indices.size());
        section.indexCount = static_cast<uint32_t>(localMesh.indices.size());

        const float worldOffsetX = static_cast<float>(worldChunk.coord.x * Chunk::SizeX);
        const float worldOffsetZ = static_cast<float>(worldChunk.coord.z * Chunk::SizeZ);

        const uint32_t vertexOffset = static_cast<uint32_t>(out.mesh.vertices.size());

        for (Vertex vertex : localMesh.vertices) {
            vertex.position[0] += worldOffsetX;
            vertex.position[2] += worldOffsetZ;
            out.mesh.vertices.push_back(vertex);
        }

        for (uint32_t index : localMesh.indices) {
            out.mesh.indices.push_back(index + vertexOffset);
        }

        out.sections.push_back(section);
    }

    return out;
}

}