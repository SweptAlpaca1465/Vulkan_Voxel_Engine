#include "world/MeshBuilder.hpp"

namespace MeshBuilder {

ChunkMesh makeTestCube() {
    ChunkMesh mesh;

    mesh.vertices = {
        // front
        {{-0.5f, -0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}},
        {{ 0.5f, -0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}},
        {{ 0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f,  0.5f,  0.5f}, {1.0f, 1.0f, 0.0f}},

        // back
        {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 1.0f}},
        {{ 0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 1.0f}},
        {{ 0.5f,  0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}},
        {{-0.5f,  0.5f, -0.5f}, {0.3f, 0.3f, 0.3f}},
    };

    mesh.indices = {
        0, 1, 2, 2, 3, 0, // front
        1, 5, 6, 6, 2, 1, // right
        5, 4, 7, 7, 6, 5, // back
        4, 0, 3, 3, 7, 4, // left
        3, 2, 6, 6, 7, 3, // top
        4, 5, 1, 1, 0, 4  // bottom
    };

    return mesh;
}

}