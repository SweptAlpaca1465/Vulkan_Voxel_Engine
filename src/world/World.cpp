#include "world/World.hpp"

#include <algorithm>

namespace {
int floorDiv(int value, int size) {
    return static_cast<int>(std::floor(static_cast<float>(value) / static_cast<float>(size)));
}

int positiveMod(int value, int size) {
    int result = value % size;
    if (result < 0) {
        result += size;
    }
    return result;
}

float lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

float smoothStep(float t) {
    return t * t * (3.0f - 2.0f * t);
}

float hashNoise(int x, int z) {
    const int h = x * 374761393 + z * 668265263;
    const int mixed = (h ^ (h >> 13)) * 1274126177;
    const int finalHash = mixed ^ (mixed >> 16);
    const float unit = static_cast<float>(finalHash & 0x7fffffff) / 2147483647.0f;
    return unit * 2.0f - 1.0f;
}

float valueNoise2D(float x, float z) {
    const int x0 = static_cast<int>(std::floor(x));
    const int z0 = static_cast<int>(std::floor(z));
    const int x1 = x0 + 1;
    const int z1 = z0 + 1;

    const float tx = smoothStep(x - static_cast<float>(x0));
    const float tz = smoothStep(z - static_cast<float>(z0));

    const float n00 = hashNoise(x0, z0);
    const float n10 = hashNoise(x1, z0);
    const float n01 = hashNoise(x0, z1);
    const float n11 = hashNoise(x1, z1);

    const float nx0 = lerp(n00, n10, tx);
    const float nx1 = lerp(n01, n11, tx);
    return lerp(nx0, nx1, tz);
}

float fbm2D(float x, float z) {
    float value = 0.0f;
    float amplitude = 1.0f;
    float frequency = 1.0f;
    float amplitudeSum = 0.0f;

    for (int octave = 0; octave < 4; ++octave) {
        value += valueNoise2D(x * frequency, z * frequency) * amplitude;
        amplitudeSum += amplitude;
        amplitude *= 0.5f;
        frequency *= 2.0f;
    }

    if (amplitudeSum > 0.0f) {
        value /= amplitudeSum;
    }

    return value;
}
}

void World::clear() {
    chunks.clear();
}

void World::generateFlatWorld(int radius) {
    chunks.clear();

    for (int cz = -radius; cz <= radius; ++cz) {
        for (int cx = -radius; cx <= radius; ++cx) {
            generateChunk(cx, cz);
        }
    }
}

void World::generateChunk(int x, int z) {
    if (hasChunk(x, z)) {
        return;
    }

    WorldChunk worldChunk;
    worldChunk.coord = { x, z };

    for (int localZ = 0; localZ < Chunk::SizeZ; ++localZ) {
        for (int localX = 0; localX < Chunk::SizeX; ++localX) {
            const int worldX = x * Chunk::SizeX + localX;
            const int worldZ = z * Chunk::SizeZ + localZ;

            const float temperature = fbm2D(static_cast<float>(worldX) * 0.010f, static_cast<float>(worldZ) * 0.010f, 4);
            const float humidity = fbm2D(static_cast<float>(worldX) * 0.014f, static_cast<float>(worldZ) * 0.014f, 4);
            const float weirdness = fbm2D(static_cast<float>(worldX) * 0.030f, static_cast<float>(worldZ) * 0.030f, 3);

            const TerrainBiome biome = chooseBiome(temperature, humidity, weirdness);
            const int terrainHeight = computeTerrainHeight(worldX, worldZ, biome);

            for (int y = 0; y <= terrainHeight; ++y) {
                BlockType type = BlockType::Stone;

                if (y == terrainHeight) {
                    type = (biome == TerrainBiome::Rocky) ? BlockType::Stone : BlockType::Grass;
                } else if (y >= terrainHeight - 2) {
                    type = (biome == TerrainBiome::Rocky) ? BlockType::Stone : BlockType::Dirt;
                }

                if (y > 1 && y < terrainHeight - 1 && shouldCarveCave(worldX, y, worldZ)) {
                    type = BlockType::Air;
                }

                worldChunk.chunk.set(localX, y, localZ, type);
            const float macroNoise = fbm2D(
                static_cast<float>(worldX) * 0.045f,
                static_cast<float>(worldZ) * 0.045f
            );
            const float detailNoise = fbm2D(
                static_cast<float>(worldX) * 0.11f,
                static_cast<float>(worldZ) * 0.11f
            );

            float heightFactor = macroNoise * 0.8f + detailNoise * 0.2f;
            heightFactor = std::clamp(heightFactor, -1.0f, 1.0f);

            const int terrainHeight = std::clamp(
                static_cast<int>(std::round(5.0f + heightFactor * 3.5f)),
                1,
                Chunk::SizeY - 2
            );

            for (int y = 0; y <= terrainHeight; ++y) {
                if (y == terrainHeight) {
                    worldChunk.chunk.set(localX, y, localZ, BlockType::Grass);
                } else if (y >= terrainHeight - 2) {
                    worldChunk.chunk.set(localX, y, localZ, BlockType::Dirt);
                } else {
                    worldChunk.chunk.set(localX, y, localZ, BlockType::Stone);
                }
            }
        }
    }

    worldChunk.chunk.markDirty();
    chunks.push_back(worldChunk);
}

bool World::hasChunk(int x, int z) const {
    return findChunk(x, z) != nullptr;
}

bool World::removeChunk(int x, int z) {
    for (auto it = chunks.begin(); it != chunks.end(); ++it) {
        if (it->coord.x == x && it->coord.z == z) {
            chunks.erase(it);
            return true;
        }
    }

    return false;
}

const std::vector<WorldChunk>& World::getChunks() const {
    return chunks;
}

std::vector<WorldChunk>& World::getChunksMutable() {
    return chunks;
}

const WorldChunk* World::findChunk(int x, int z) const {
    for (const WorldChunk& chunk : chunks) {
        if (chunk.coord.x == x && chunk.coord.z == z) {
            return &chunk;
        }
    }

    return nullptr;
}

WorldChunk* World::findChunkMutable(int x, int z) {
    for (WorldChunk& chunk : chunks) {
        if (chunk.coord.x == x && chunk.coord.z == z) {
            return &chunk;
        }
    }

    return nullptr;
}

void World::markChunkDirty(int x, int z) {
    WorldChunk* chunk = findChunkMutable(x, z);
    if (chunk) {
        chunk->chunk.markDirty();
    }
}

bool World::hasDirtyChunks() const {
    for (const WorldChunk& worldChunk : chunks) {
        if (worldChunk.chunk.isDirty()) {
            return true;
        }
    }

    return false;
}

bool World::setBlock(int chunkX, int chunkZ, int localX, int y, int localZ, BlockType type) {
    WorldChunk* chunk = findChunkMutable(chunkX, chunkZ);
    if (!chunk) {
        return false;
    }

    chunk->chunk.set(localX, y, localZ, type);

    if (localX == 0) {
        markChunkDirty(chunkX - 1, chunkZ);
    }
    if (localX == Chunk::SizeX - 1) {
        markChunkDirty(chunkX + 1, chunkZ);
    }
    if (localZ == 0) {
        markChunkDirty(chunkX, chunkZ - 1);
    }
    if (localZ == Chunk::SizeZ - 1) {
        markChunkDirty(chunkX, chunkZ + 1);
    }

    return true;
}

bool World::getBlockGlobal(int worldX, int y, int worldZ, BlockType& outBlock) const {
    if (y < 0 || y >= Chunk::SizeY) {
        outBlock = BlockType::Air;
        return false;
    }

    const int chunkX = floorDiv(worldX, Chunk::SizeX);
    const int chunkZ = floorDiv(worldZ, Chunk::SizeZ);

    const int localX = positiveMod(worldX, Chunk::SizeX);
    const int localZ = positiveMod(worldZ, Chunk::SizeZ);

    const WorldChunk* chunk = findChunk(chunkX, chunkZ);
    if (!chunk) {
        outBlock = BlockType::Air;
        return false;
    }

    outBlock = chunk->chunk.get(localX, y, localZ);
    return true;
}

bool World::setBlockGlobal(int worldX, int y, int worldZ, BlockType type) {
    if (y < 0 || y >= Chunk::SizeY) {
        return false;
    }

    const int chunkX = floorDiv(worldX, Chunk::SizeX);
    const int chunkZ = floorDiv(worldZ, Chunk::SizeZ);

    const int localX = positiveMod(worldX, Chunk::SizeX);
    const int localZ = positiveMod(worldZ, Chunk::SizeZ);

    return setBlock(chunkX, chunkZ, localX, y, localZ, type);
}
