#pragma once

#include "vulkan/vulkan.h"

#include <filesystem>

class VulkanDevice;

class Texture
{
public:
    Texture(VulkanDevice* device, const std::filesystem::path& path);
    ~Texture();

    Texture(const Texture&) = delete;
    Texture operator = (const Texture&) = delete;

    inline VkImage GetImage() const { return m_Image; }
    inline VkImageView GetImageView() const { return m_ImageView; }
private:
    VulkanDevice* m_Device;
    uint32_t m_Width, m_Height;
    VkImage m_Image = VK_NULL_HANDLE;
    VkDeviceMemory m_ImageMemory = VK_NULL_HANDLE;
    VkImageView m_ImageView = VK_NULL_HANDLE;

    void CreateImageView();
};
