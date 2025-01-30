#pragma once

#include "vulkan/vulkan.h"

#include <vector>

class VulkanDevice;

class Framebuffer
{
public:
    Framebuffer(VulkanDevice* device, VkRenderPass renderPass, const std::vector<VkImageView>& attachments, VkExtent2D extent);
    ~Framebuffer();

    Framebuffer(const Framebuffer&) = delete;
    Framebuffer operator = (const Framebuffer&) = delete;

    inline VkFramebuffer Get() const { return m_Framebuffer; }
private:
    VulkanDevice* m_Device;
    VkFramebuffer m_Framebuffer = nullptr;
};
