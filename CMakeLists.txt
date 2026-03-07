cmake_minimum_required(VERSION 3.20)
project(VulkanVoxelEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

add_executable(VulkanVoxelEngine
    src/main.cpp
    src/core/App.cpp
    src/core/Window.cpp
    src/renderer/Renderer.cpp
    src/world/Chunk.cpp
    src/world/MeshBuilder.cpp
    src/world/ChunkMesher.cpp
    src/world/World.cpp
    src/world/WorldMesher.cpp
    src/camera/Camera.cpp
)

target_include_directories(VulkanVoxelEngine PRIVATE include)

target_link_libraries(VulkanVoxelEngine PRIVATE
    Vulkan::Vulkan
    glfw
    glm::glm
)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(VulkanVoxelEngine PRIVATE
        -Wall -Wextra -Wpedantic
    )
endif()