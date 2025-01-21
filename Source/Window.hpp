#pragma once

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

class Window
{
public:
    Window(uint32_t width, uint32_t height, const char* title);
    ~Window();

    Window(const Window&) = delete;
    Window operator = (const Window&) = delete;

    bool ShouldClose() const;
    bool ResizeFlag = false;

    inline GLFWwindow* Get() const { return m_Window; }
    inline uint32_t GetWidth() const { return m_Width; }
    inline uint32_t GetHeight() const { return m_Height; }

    VkSurfaceKHR CreateWindowSurface(VkInstance instance);
private:
    GLFWwindow* m_Window;

    uint32_t m_Width;
    uint32_t m_Height;

    static void framebuffer_resize_callback(GLFWwindow* window, int width, int height);
};
