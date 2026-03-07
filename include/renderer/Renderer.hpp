#pragma once

class Window;

class Renderer {
public:
    explicit Renderer(Window& window);
    ~Renderer();

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    void drawFrame();
    void waitIdle();

private:
    class Impl;
    Impl* impl = nullptr;
};