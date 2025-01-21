#pragma once

#include "vulkan/vulkan.h"

#include <vector>

static const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

std::vector<const char*> GetRequiredExtensions();
VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
