#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "glm.hpp"

#include <string>

struct GLFWwindow;

namespace lv
{
    class Window
    {
    public:
        Window(const glm::uvec2 size, const std::string& title);
        ~Window();

        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;

        bool ShouldClose() const;
        void CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
        VkExtent2D GetExtent() const { return {m_Size.x, m_Size.y}; }
        glm::uvec2 GetSize() const { return m_Size; }
    private:
        glm::uvec2 m_Size;
        std::string m_Title;
        GLFWwindow* m_GLFWWindow;
    };
}

