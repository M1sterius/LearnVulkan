#pragma once

#include "vulkan/vulkan.h"

class VulkanDevice;

class Semaphore
{
public:
    explicit Semaphore(VulkanDevice* device);
    ~Semaphore();

    Semaphore(const Semaphore&) = delete;
    Semaphore operator = (const Semaphore&) = delete;

    inline VkSemaphore Get() const { return m_Semaphore; }
private:
    VulkanDevice* m_Device;
    VkSemaphore m_Semaphore = VK_NULL_HANDLE;
};
