#pragma once

#include "vulkan/vulkan.h"

#include <memory>
#include <vector>

class VulkanDevice;

class Swapchain
{
public:
    Swapchain(VulkanDevice* device, VkSurfaceKHR surface, VkExtent2D extent);
    ~Swapchain();

    Swapchain(const Swapchain&) = delete;
    Swapchain operator = (const Swapchain&) = delete;

    static constexpr uint32_t FramesInFlight = 2;

    inline VkSwapchainKHR Get() const { return m_Swapchain; }
    inline VkFormat GetFormat() const { return m_SwapChainFormat; }
    inline VkExtent2D GetExtent() const { return m_SwapChainExtent; }
    std::vector<VkImage>& GetImages() { return m_Images; }
    std::vector<VkImageView>& GetImageViews() { return m_ImageViews; }

    inline uint32_t GetCurrentFrameIndex() const { return m_FrameIndex; }
    VkResult AcquireNextImage(uint32_t* imageIndex);
    void SubmitCommandBuffer(VkCommandBuffer commandBuffer);
    VkResult Present(uint32_t imageIndex);
    void ResetFence();

    void Recreate(VkExtent2D newExtent);
private:
    void CreateSwapchain(VkExtent2D extent);
    void CreateImageViews();
    void CreateSyncObjects();
    void CleanupSwapchain();

    VulkanDevice* m_Device = nullptr;
    VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
    VkSwapchainKHR m_Swapchain = VK_NULL_HANDLE;
    VkFormat m_SwapChainFormat {};
    VkExtent2D m_SwapChainExtent {};
    uint32_t m_FrameIndex = 0;
    std::vector<VkImage> m_Images;
    std::vector<VkImageView> m_ImageViews;

    std::vector<VkSemaphore> m_ImageAvailableSemaphores;
    std::vector<VkSemaphore> m_RenderFinishedSemaphores;
    std::vector<VkFence> m_InFlightFences;

    static VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    static VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    static VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, VkExtent2D extent);
};
