#pragma once

#include "vulkan/vulkan.h"

class VulkanDevice;

class UniformBuffer
{
public:
    UniformBuffer(VulkanDevice* device, VkDeviceSize bufferSize);
    ~UniformBuffer();

    UniformBuffer(const UniformBuffer&) = delete;
    UniformBuffer operator = (const UniformBuffer&) = delete;
private:
    VulkanDevice* m_Device;
    VkDeviceSize m_BufferSize;
    VkBuffer m_Buffer = VK_NULL_HANDLE;
    VkDeviceMemory m_BufferMemory = VK_NULL_HANDLE;
    void* m_BufferDataMap = nullptr;
};
