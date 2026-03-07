#pragma once

#include "core/Window.hpp"
#include "renderer/Renderer.hpp"

class App {
public:
    App();
    void run();

private:
    Window window;
    Renderer renderer;
};