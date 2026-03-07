#pragma once

#include "renderer/Vertex.hpp"

#include <cstdint>
#include <vector>

struct ChunkMesh {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    bool empty() const {
        return vertices.empty() || indices.empty();
    }
};