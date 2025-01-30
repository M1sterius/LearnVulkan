#include "Semaphore.hpp"
#include "VulkanDevice.hpp"

Semaphore::Semaphore(VulkanDevice* device)
    :   m_Device(device)
{
    VkSemaphoreCreateInfo semaphoreInfo { };
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    if (vkCreateSemaphore(m_Device->Get(), &semaphoreInfo, nullptr, &m_Semaphore) != VK_SUCCESS)
        throw std::runtime_error("Failed to create semaphore!");
}

Semaphore::~Semaphore()
{
    vkDestroySemaphore(m_Device->Get(), m_Semaphore, nullptr);
}
