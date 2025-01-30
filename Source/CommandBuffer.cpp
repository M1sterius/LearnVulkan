#include "CommandBuffer.hpp"
#include "VulkanDevice.hpp"

CommandBuffer::CommandBuffer(VulkanDevice* device)
    :   m_Device(device)
{
    VkCommandBufferAllocateInfo allocInfo { };
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_Device->GetCommandPool();
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(m_Device->Get(), &allocInfo, &m_CommandBuffer) != VK_SUCCESS)
        throw std::runtime_error("Failed to allocate command buffers!");
}

CommandBuffer::~CommandBuffer()
{
    vkFreeCommandBuffers(m_Device->Get(), m_Device->GetCommandPool(), 1, &m_CommandBuffer);
}

void CommandBuffer::BeginRecording(VkCommandBufferUsageFlags flags)
{
    VkCommandBufferBeginInfo beginInfo {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = flags;

    if (vkBeginCommandBuffer(m_CommandBuffer, &beginInfo) != VK_SUCCESS)
        throw std::runtime_error("Failed to begin recording command buffer!");
}

void CommandBuffer::EndRecording()
{
    if (vkEndCommandBuffer(m_CommandBuffer) != VK_SUCCESS)
        throw std::runtime_error("Failed to record command buffer!");
}

void CommandBuffer::Reset(VkCommandBufferResetFlags flags)
{
    vkResetCommandBuffer(m_CommandBuffer, flags);
}
