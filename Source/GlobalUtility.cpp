#include "GlobalUtility.hpp"
#include "GLFW/glfw3.h"

#include <cstdint>

std::vector<const char*> GetRequiredExtensions()
{
    uint32_t glfwExtensionCount = 0;
    auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    return extensions;
}
