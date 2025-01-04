#include "Window.hpp"
#include <GLFW/glfw3.h>

#include <stdexcept>

namespace lv
{

    Window::Window(const glm::uvec2 size, const std::string& title)
        : m_Size(size), m_Title(title)
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        m_GLFWWindow = glfwCreateWindow((int)m_Size.x, (int)m_Size.y,
                    title.c_str(), nullptr, nullptr);

        glfwSetWindowUserPointer(m_GLFWWindow, this);
        glfwSetFramebufferSizeCallback(m_GLFWWindow, FramebufferResizeCallback);
    }

    Window::~Window()
    {
        glfwDestroyWindow(m_GLFWWindow);
        glfwTerminate();
    }

    bool Window::ShouldClose() const
    {
        return glfwWindowShouldClose(m_GLFWWindow);
    }

    void Window::CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
    {
        if (glfwCreateWindowSurface(instance, m_GLFWWindow, nullptr, surface) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create window surface");
        }
    }

    void Window::FramebufferResizeCallback(GLFWwindow* window, int width, int height)
    {
        auto win = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
        win->m_IsFramebufferResized = true;
        win->m_Size = {width, height};
    }
}