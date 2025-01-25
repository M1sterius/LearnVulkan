#include "IndexBuffer.hpp"
#include "VulkanDevice.hpp"

IndexBuffer::IndexBuffer(VulkanDevice* device, const std::vector<uint32_t>& indices)
    :   m_Device(device)
{

}

IndexBuffer::~IndexBuffer()
{

}
