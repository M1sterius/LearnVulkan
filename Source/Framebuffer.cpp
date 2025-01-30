#include "Framebuffer.hpp"
#include "VulkanDevice.hpp"

Framebuffer::Framebuffer(VulkanDevice* device, VkRenderPass renderPass, const std::vector<VkImageView>& attachments,
                         VkExtent2D extent)
    :   m_Device(device)
{
    VkFramebufferCreateInfo framebufferInfo {};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = renderPass;
    framebufferInfo.attachmentCount = attachments.size();
    framebufferInfo.pAttachments = attachments.data();
    framebufferInfo.width = extent.width;
    framebufferInfo.height = extent.height;
    framebufferInfo.layers = 1;

    if (vkCreateFramebuffer(m_Device->Get(), &framebufferInfo, nullptr, &m_Framebuffer) != VK_SUCCESS)
        throw std::runtime_error("Failed to create framebuffer!");
}

Framebuffer::~Framebuffer()
{
    vkDestroyFramebuffer(m_Device->Get(), m_Framebuffer, nullptr);
}
