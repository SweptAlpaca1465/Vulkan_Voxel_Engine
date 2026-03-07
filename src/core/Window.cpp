#include "core/Window.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <stdexcept>

Window::Window(int width, int height, const std::string& title)
    : width(width), height(height) {
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }

    glfwShowWindow(window);
}

Window::~Window() {
    if (window) {
        glfwDestroyWindow(window);
    }
    glfwTerminate();
}

GLFWwindow* Window::getNativeHandle() const {
    return window;
}

bool Window::shouldClose() const {
    return glfwWindowShouldClose(window);
}

void Window::pollEvents() const {
    glfwPollEvents();
}

int Window::getWidth() const {
    return width;
}

int Window::getHeight() const {
    return height;
}

void Window::setTitle(const std::string& title) const {
    glfwSetWindowTitle(window, title.c_str());
}