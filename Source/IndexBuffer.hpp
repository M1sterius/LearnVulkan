#pragma once

#include "vulkan/vulkan.h"

#include <vector>

class VulkanDevice;

class IndexBuffer
{
public:
    IndexBuffer(VulkanDevice* device, const std::vector<uint32_t>& indices);
    ~IndexBuffer();

    IndexBuffer(const IndexBuffer&) = delete;
    IndexBuffer operator = (const IndexBuffer&) = delete;
private:
    VulkanDevice* m_Device;
    VkBuffer m_Buffer = VK_NULL_HANDLE;
    VkDeviceMemory m_BufferMemory = VK_NULL_HANDLE;
};
