#include "core/App.hpp"

App::App()
    : window(1280, 720, "Vulkan Voxel Engine"),
      renderer(window) {
}

void App::run() {
    while (!window.shouldClose()) {
        window.pollEvents();
        renderer.drawFrame();
    }

    renderer.waitIdle();
}