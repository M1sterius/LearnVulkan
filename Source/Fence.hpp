#pragma once

#include "vulkan/vulkan.h"

class VulkanDevice;

class Fence
{
public:
    explicit Fence(VulkanDevice* device, bool signalBit = false);
    ~Fence();

    Fence(const Fence&) = delete;
    Fence operator = (const Fence&) = delete;

    inline VkFence Get() const { return m_Fence; }

    void Reset();
    void Wait(uint64_t timeout = UINT64_MAX);
private:
    VulkanDevice* m_Device;
    VkFence m_Fence = VK_NULL_HANDLE;
};
