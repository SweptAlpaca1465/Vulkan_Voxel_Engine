#pragma once

#include <string>

struct GLFWwindow;

class Window {
public:
    Window(int width, int height, const std::string& title);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    GLFWwindow* getNativeHandle() const;

    bool shouldClose() const;
    void pollEvents() const;

    int getWidth() const;
    int getHeight() const;

    void setTitle(const std::string& title) const;

private:
    GLFWwindow* window = nullptr;
    int width = 0;
    int height = 0;
};