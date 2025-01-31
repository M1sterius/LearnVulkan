#include "IndexBuffer.hpp"
#include "VulkanDevice.hpp"

IndexBuffer::IndexBuffer(VulkanDevice* device, const std::vector<uint32_t>& indices)
    :   m_Device(device), m_IndicesCount(indices.size())
{
    VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    m_Device->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    m_Device->MapMemory(stagingBufferMemory, bufferSize, 0, 0, (void*)indices.data());

    m_Device->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                           VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_Buffer, m_BufferMemory);

    m_Device->CopyBuffer(stagingBuffer, m_Buffer, bufferSize);

    vkDestroyBuffer(device->Get(), stagingBuffer, nullptr);
    vkFreeMemory(device->Get(), stagingBufferMemory, nullptr);
}

IndexBuffer::~IndexBuffer()
{
    vkDestroyBuffer(m_Device->Get(), m_Buffer, nullptr);
    vkFreeMemory(m_Device->Get(), m_BufferMemory, nullptr);
}
