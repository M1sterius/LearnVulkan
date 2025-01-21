#include <stdexcept>
#include "Window.hpp"

Window::Window(uint32_t width, uint32_t height, const char* title)
    :   m_Width(width), m_Height(height)
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    m_Window = glfwCreateWindow((int)width, (int)height,
                                title, nullptr, nullptr);

    glfwSetWindowUserPointer(m_Window, this);
    glfwSetFramebufferSizeCallback(m_Window, framebuffer_resize_callback);
}

Window::~Window()
{
    glfwDestroyWindow(m_Window);
    glfwTerminate();
}

bool Window::ShouldClose() const
{
    return glfwWindowShouldClose(m_Window);
}

void Window::framebuffer_resize_callback(GLFWwindow* window, int width, int height)
{
    auto win = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
    win->ResizeFlag = true;
    win->m_Width = static_cast<uint32_t>(width);
    win->m_Height = static_cast<uint32_t>(height);
}

VkSurfaceKHR Window::CreateWindowSurface(VkInstance instance)
{
    VkSurfaceKHR surface = VK_NULL_HANDLE;

    if (glfwCreateWindowSurface(instance, m_Window, nullptr, &surface) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create window surface!");
    }

    return surface;
}

