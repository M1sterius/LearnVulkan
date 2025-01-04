#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Window.hpp"
#include "Pipeline.hpp"
#include "SwapChain.hpp"
#include "Device.hpp"
#include "App.hpp"

#include <cstdlib>
#include <iostream>

int32_t main(int32_t argc, char* argv[])
{
    constexpr glm::uvec2 WINDOW_SIZE = {1280, 720};

    auto app = lv::App(WINDOW_SIZE);
    app.Run();
}

