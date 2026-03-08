#include "world/ChunkMesher.hpp"

#include <array>
#include <vector>

namespace ChunkMesher {
namespace {

struct MaskCell {
    BlockType block = BlockType::Air;
    int normal = 0;

    bool operator==(const MaskCell& other) const {
        return block == other.block && normal == other.normal;
    }

    bool isEmpty() const {
        return block == BlockType::Air || normal == 0;
    }
};

std::array<float, 3> getBaseColor(BlockType block) {
    switch (block) {
        case BlockType::Grass:
            return {0.25f, 0.80f, 0.25f};
        case BlockType::Dirt:
            return {0.55f, 0.27f, 0.07f};
        case BlockType::Stone:
            return {0.55f, 0.55f, 0.60f};
        case BlockType::Air:
        default:
            return {1.0f, 1.0f, 1.0f};
    }
}

float getFaceShade(int axis, int normal) {
    if (axis == 1) {
        return (normal > 0) ? 1.00f : 0.55f; // top / bottom
    }

    if (axis == 0) {
        return 0.85f; // x-facing sides
    }

    return 0.70f; // z-facing sides
}

Vertex makeVertex(float x, float y, float z, BlockType block, float shade) {
    std::array<float, 3> color = getBaseColor(block);

    color[0] *= shade;
    color[1] *= shade;
    color[2] *= shade;

    return Vertex{{x, y, z}, color};
}

bool isSolid(BlockType block) {
    return block != BlockType::Air;
}

BlockType sampleBlock(
    const Chunk& chunk,
    const Chunk* west,
    const Chunk* east,
    const Chunk* north,
    const Chunk* south,
    int x,
    int y,
    int z
) {
    if (y < 0 || y >= Chunk::SizeY) {
        return BlockType::Air;
    }

    if (x < 0) {
        if (!west) return BlockType::Air;
        return west->get(Chunk::SizeX - 1, y, z);
    }

    if (x >= Chunk::SizeX) {
        if (!east) return BlockType::Air;
        return east->get(0, y, z);
    }

    if (z < 0) {
        if (!north) return BlockType::Air;
        return north->get(x, y, Chunk::SizeZ - 1);
    }

    if (z >= Chunk::SizeZ) {
        if (!south) return BlockType::Air;
        return south->get(x, y, 0);
    }

    return chunk.get(x, y, z);
}

void addQuad(
    ChunkMesh& mesh,
    BlockType block,
    int axis,
    int slice,
    int u0,
    int v0,
    int width,
    int height,
    int normal
) {
    const uint32_t startIndex = static_cast<uint32_t>(mesh.vertices.size());
    const float shade = getFaceShade(axis, normal);

    std::array<std::array<float, 3>, 4> positions{};

    if (axis == 0) {
        const float x = static_cast<float>(slice);

        if (normal > 0) {
            positions[0] = {x, static_cast<float>(u0), static_cast<float>(v0)};
            positions[1] = {x, static_cast<float>(u0 + width), static_cast<float>(v0)};
            positions[2] = {x, static_cast<float>(u0 + width), static_cast<float>(v0 + height)};
            positions[3] = {x, static_cast<float>(u0), static_cast<float>(v0 + height)};
        } else {
            positions[0] = {x, static_cast<float>(u0), static_cast<float>(v0 + height)};
            positions[1] = {x, static_cast<float>(u0 + width), static_cast<float>(v0 + height)};
            positions[2] = {x, static_cast<float>(u0 + width), static_cast<float>(v0)};
            positions[3] = {x, static_cast<float>(u0), static_cast<float>(v0)};
        }
    } else if (axis == 1) {
        const float y = static_cast<float>(slice);

        if (normal > 0) {
            positions[0] = {static_cast<float>(u0), y, static_cast<float>(v0)};
            positions[1] = {static_cast<float>(u0), y, static_cast<float>(v0 + height)};
            positions[2] = {static_cast<float>(u0 + width), y, static_cast<float>(v0 + height)};
            positions[3] = {static_cast<float>(u0 + width), y, static_cast<float>(v0)};
        } else {
            positions[0] = {static_cast<float>(u0), y, static_cast<float>(v0)};
            positions[1] = {static_cast<float>(u0 + width), y, static_cast<float>(v0)};
            positions[2] = {static_cast<float>(u0 + width), y, static_cast<float>(v0 + height)};
            positions[3] = {static_cast<float>(u0), y, static_cast<float>(v0 + height)};
        }
    } else {
        const float z = static_cast<float>(slice);

        if (normal > 0) {
            positions[0] = {static_cast<float>(u0), static_cast<float>(v0), z};
            positions[1] = {static_cast<float>(u0 + width), static_cast<float>(v0), z};
            positions[2] = {static_cast<float>(u0 + width), static_cast<float>(v0 + height), z};
            positions[3] = {static_cast<float>(u0), static_cast<float>(v0 + height), z};
        } else {
            positions[0] = {static_cast<float>(u0), static_cast<float>(v0 + height), z};
            positions[1] = {static_cast<float>(u0 + width), static_cast<float>(v0 + height), z};
            positions[2] = {static_cast<float>(u0 + width), static_cast<float>(v0), z};
            positions[3] = {static_cast<float>(u0), static_cast<float>(v0), z};
        }
    }

    for (const auto& p : positions) {
        mesh.vertices.push_back(makeVertex(p[0], p[1], p[2], block, shade));
    }

    mesh.indices.push_back(startIndex + 0);
    mesh.indices.push_back(startIndex + 1);
    mesh.indices.push_back(startIndex + 2);
    mesh.indices.push_back(startIndex + 2);
    mesh.indices.push_back(startIndex + 3);
    mesh.indices.push_back(startIndex + 0);
}

int axisSize(int axis) {
    if (axis == 0) return Chunk::SizeX;
    if (axis == 1) return Chunk::SizeY;
    return Chunk::SizeZ;
}

BlockType getBlockByAxis(
    const Chunk& chunk,
    const Chunk* west,
    const Chunk* east,
    const Chunk* north,
    const Chunk* south,
    int axis,
    int a,
    int b,
    int c
) {
    if (axis == 0) return sampleBlock(chunk, west, east, north, south, a, b, c);
    if (axis == 1) return sampleBlock(chunk, west, east, north, south, b, a, c);
    return sampleBlock(chunk, west, east, north, south, b, c, a);
}

} // namespace

ChunkMesh build(
    const Chunk& chunk,
    const Chunk* west,
    const Chunk* east,
    const Chunk* north,
    const Chunk* south
) {
    ChunkMesh mesh;

    for (int axis = 0; axis < 3; ++axis) {
        const int dimA = axisSize(axis);
        const int dimU = axisSize((axis + 1) % 3);
        const int dimV = axisSize((axis + 2) % 3);

        std::vector<MaskCell> mask(static_cast<size_t>(dimU * dimV));

        for (int slice = 0; slice <= dimA; ++slice) {
            for (int v = 0; v < dimV; ++v) {
                for (int u = 0; u < dimU; ++u) {
                    BlockType backBlock = getBlockByAxis(chunk, west, east, north, south, axis, slice - 1, u, v);
                    BlockType frontBlock = getBlockByAxis(chunk, west, east, north, south, axis, slice, u, v);

                    const bool backSolid = isSolid(backBlock);
                    const bool frontSolid = isSolid(frontBlock);

                    MaskCell cell{};

                    if (backSolid && !frontSolid) {
                        cell.block = backBlock;
                        cell.normal = +1;
                    } else if (!backSolid && frontSolid) {
                        cell.block = frontBlock;
                        cell.normal = -1;
                    } else {
                        cell.block = BlockType::Air;
                        cell.normal = 0;
                    }

                    mask[static_cast<size_t>(u + v * dimU)] = cell;
                }
            }

            for (int v = 0; v < dimV; ++v) {
                for (int u = 0; u < dimU;) {
                    const MaskCell cell = mask[static_cast<size_t>(u + v * dimU)];

                    if (cell.isEmpty()) {
                        ++u;
                        continue;
                    }

                    int width = 1;
                    while (u + width < dimU &&
                           mask[static_cast<size_t>((u + width) + v * dimU)] == cell) {
                        ++width;
                    }

                    int height = 1;
                    bool canGrow = true;

                    while (v + height < dimV && canGrow) {
                        for (int k = 0; k < width; ++k) {
                            if (!(mask[static_cast<size_t>((u + k) + (v + height) * dimU)] == cell)) {
                                canGrow = false;
                                break;
                            }
                        }

                        if (canGrow) {
                            ++height;
                        }
                    }

                    addQuad(mesh, cell.block, axis, slice, u, v, width, height, cell.normal);

                    for (int dy = 0; dy < height; ++dy) {
                        for (int dx = 0; dx < width; ++dx) {
                            mask[static_cast<size_t>((u + dx) + (v + dy) * dimU)] = {};
                        }
                    }

                    u += width;
                }
            }
        }
    }

    return mesh;
}

} // namespace ChunkMesher