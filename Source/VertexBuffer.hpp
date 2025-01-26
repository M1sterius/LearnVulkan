#pragma once

#include "vulkan/vulkan.h"
#include "glm.hpp"

#include <array>
#include <vector>

class VulkanDevice;

struct Vertex
{
    glm::vec3 position;
    glm::vec3 color;

    inline static VkVertexInputBindingDescription GetBindingDescription()
    {
        /*
         * Apparently it specifies how many vertex buffers will be used inside the shader
         * Something like bind multiple VBOs to a single VAO in OpenGL
         * There we use only one vertex buffer in the shader so we have a single binding description
         */
        VkVertexInputBindingDescription bindingDescription {};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    inline static std::array<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions()
    {
        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions {};

        // a_Position (location = 0)
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, position);

        // a_Color (location = 1)
        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        return attributeDescriptions;
    }
};

class VertexBuffer
{
public:
    VertexBuffer(VulkanDevice* device, const std::vector<Vertex>& vertices);
    ~VertexBuffer();

    VertexBuffer(const VertexBuffer&) = delete;
    VertexBuffer operator = (const VertexBuffer&) = delete;

    inline VkBuffer Get() const { return m_Buffer; }
    inline VkDeviceMemory GetMemory() const { return m_BufferMemory; }
    inline uint32_t GetVerticesCount() const { return m_VerticesCount; }
private:
    VulkanDevice* m_Device;
    VkBuffer m_Buffer = VK_NULL_HANDLE;
    VkDeviceMemory m_BufferMemory = VK_NULL_HANDLE;
    uint32_t m_VerticesCount = 0;
};
