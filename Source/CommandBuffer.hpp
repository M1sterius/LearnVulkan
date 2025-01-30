#pragma once

#include "vulkan/vulkan.h"

class VulkanDevice;

class CommandBuffer
{
public:
    explicit CommandBuffer(VulkanDevice* device);
    ~CommandBuffer();

    CommandBuffer(const CommandBuffer&) = delete;
    CommandBuffer operator = (const CommandBuffer&) = delete;

    inline VkCommandBuffer Get() const { return m_CommandBuffer; }

    void BeginRecording(VkCommandBufferUsageFlags flags);
    void EndRecording();
    void Reset(VkCommandBufferResetFlags flags);
private:
    VulkanDevice* m_Device;
    VkCommandBuffer m_CommandBuffer = VK_NULL_HANDLE;
};
