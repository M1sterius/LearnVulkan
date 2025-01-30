#pragma once

#include "vulkan/vulkan.h"

class VulkanDevice;

class Fence
{
public:
    explicit Fence(VulkanDevice* device ,bool signalBit = false);
    ~Fence();

    Fence(const Fence&) = delete;
    Fence operator = (const Fence&) = delete;

    void Reset();
private:
    VulkanDevice* m_Device;
    VkFence m_Fence = VK_NULL_HANDLE;
};
