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
}

Framebuffer::~Framebuffer()
{
    vkDestroyFramebuffer(m_Device->Get(), m_Framebuffer, nullptr);
}
