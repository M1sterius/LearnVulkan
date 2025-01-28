#pragma once

#include "vulkan/vulkan.h"

#include <vector>
#include <utility>
#include <cstdint>
#include <optional>

static const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

// Extensions that will be enabled
static const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

std::vector<const char*> GetRequiredExtensions();