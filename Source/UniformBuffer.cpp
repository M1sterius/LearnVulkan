#include "UniformBuffer.hpp"

UniformBuffer::UniformBuffer(VulkanDevice* device, VkDeviceSize bufferSize)
    :   m_Device(device), m_BufferSize(bufferSize)
{

}

UniformBuffer::~UniformBuffer()
{

}
