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

struct QueueFamilyIndices
{
    // If a device does not support a queue family, the corresponding std::optional won't have a value

    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool IsComplete() const
    {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities { };
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);

std::vector<const char*> GetRequiredExtensions();
