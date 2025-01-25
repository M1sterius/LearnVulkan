#include "VertexBuffer.hpp"
#include "VulkanDevice.hpp"

VertexBuffer::VertexBuffer(VulkanDevice* device, const std::vector<Vertex>& vertices)
    :   m_Device(device)
{
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

    // Create temporarily staging buffer so that we can upload vertex data into more efficient GPU memory
    // This buffer can only be used as transfer source
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    m_Device->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    // Upload vertex data into the staging buffer
    void* data;
    vkMapMemory(m_Device->Get(), stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, vertices.data(), (size_t)bufferSize);
    vkUnmapMemory(m_Device->Get(), stagingBufferMemory);

    // Now create the actual vertex buffer with the most optimal GPU memory type (VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
    // This buffer can be used both as transfer destination and as vertex buffer
    m_Device->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_Buffer, m_BufferMemory);

    m_Device->CopyBuffer(stagingBuffer, m_Buffer, bufferSize);

    vkDestroyBuffer(m_Device->Get(), stagingBuffer, nullptr);
    vkFreeMemory(m_Device->Get(), stagingBufferMemory, nullptr);
}

VertexBuffer::~VertexBuffer()
{
    vkDestroyBuffer(m_Device->Get(), m_Buffer, nullptr);
    vkFreeMemory(m_Device->Get(), m_BufferMemory, nullptr);
}
