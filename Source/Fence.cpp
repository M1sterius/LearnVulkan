#include "Fence.hpp"
#include "VulkanDevice.hpp"

Fence::Fence(VulkanDevice* device , bool signalBit)
    :   m_Device(device)
{
    VkFenceCreateInfo fenceInfo { };
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    if (signalBit) fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    if (vkCreateFence(m_Device->Get(), &fenceInfo, nullptr, &m_Fence) != VK_SUCCESS)
        throw std::runtime_error("Failed to create fence!");
}

Fence::~Fence()
{
    vkDestroyFence(m_Device->Get(), m_Fence, nullptr);
}

void Fence::Reset()
{
    vkResetFences(m_Device->Get(), 1, &m_Fence);
}
