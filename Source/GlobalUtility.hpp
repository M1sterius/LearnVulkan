#pragma once

#include "vulkan/vulkan.h"

#include <vector>

static const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

std::vector<const char*> GetRequiredExtensions();