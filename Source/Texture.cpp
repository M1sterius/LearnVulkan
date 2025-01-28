#include "VulkanDevice.hpp"
#include "Texture.hpp"
#include "stb_image.h"

Texture::Texture(VulkanDevice* device, const std::filesystem::path& path)
    :   m_Device(device)
{

    // Load a texture from file into a CPU side buffer
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load(path.string().c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    m_Width = static_cast<uint32_t>(texWidth);
    m_Height = static_cast<uint32_t>(texHeight);

    VkDeviceSize imageSize = m_Width * m_Height * 4; // 4 bytes for 8-bit RGBA

    if (!pixels)
        throw std::runtime_error("Failed to load texture image!");

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    // Load the data from file into the staging buffer
    m_Device->CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
    m_Device->MapMemory(stagingBufferMemory, imageSize, 0, 0, pixels);

    // Free CPU side buffer
    stbi_image_free(pixels);

    // Create image in the format that allows pixel data to be copied from the staging buffer
    m_Device->CreateImage(m_Width, m_Height, VK_FORMAT_R8G8B8A8_SRGB,
                  VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_Image, m_ImageMemory);

    // Transition the image into the layout optimal for transferring pixel data from the staging buffer into the image
    m_Device->TransitionImageLayout(m_Image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    m_Device->CopyBufferToImage(stagingBuffer, m_Image, m_Width, m_Height);

    // Transition the image into the format optimal for using it as a texture sampled from the shader
    m_Device->TransitionImageLayout(m_Image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    // Clean up the staging buffer
    vkDestroyBuffer(m_Device->Get(), stagingBuffer, nullptr);
    vkFreeMemory(m_Device->Get(), stagingBufferMemory, nullptr);

    CreateImageView();
}

Texture::~Texture()
{
    vkDestroyImageView(m_Device->Get(), m_ImageView, nullptr);
    vkDestroyImage(m_Device->Get(), m_Image, nullptr);
    vkFreeMemory(m_Device->Get(), m_ImageMemory, nullptr);
}

void Texture::CreateImageView()
{
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = m_Image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(m_Device->Get(), &viewInfo, nullptr, &m_ImageView) != VK_SUCCESS)
        throw std::runtime_error("failed to create texture image view!");
}