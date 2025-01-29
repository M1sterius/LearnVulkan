#include "Swapchain.hpp"
#include "VulkanDevice.hpp"

#include <algorithm>
#include <limits>
#include <array>

Swapchain::Swapchain(VulkanDevice* device, VkSurfaceKHR surface, VkExtent2D extent)
    :   m_Device(device), m_Surface(surface)
{
    CreateSwapchain(extent);
    CreateImageViews();
    CreateSyncObjects();
}

Swapchain::~Swapchain()
{
    CleanupSwapchain();
}

void Swapchain::CreateSwapchain(VkExtent2D extent)
{
    auto swapChainSupport = m_Device->GetSwapChainSupportDetails();
    auto surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
    auto presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
    auto chosenExtent = ChooseSwapExtent(swapChainSupport.capabilities, extent);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo { };
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = m_Surface;

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = chosenExtent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    const auto indices = m_Device->GetQueueFamilyIndices();
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    if (indices.graphicsFamily != indices.presentFamily)
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(m_Device->Get(), &createInfo, nullptr, &m_Swapchain) != VK_SUCCESS)
        throw std::runtime_error("Failed to create swap chain!");

    // Create swapchain images
    vkGetSwapchainImagesKHR(m_Device->Get(), m_Swapchain, &imageCount, nullptr);
    m_Images.resize(imageCount);
    vkGetSwapchainImagesKHR(m_Device->Get(), m_Swapchain, &imageCount, m_Images.data());

    m_SwapChainFormat = surfaceFormat.format;
    m_SwapChainExtent = chosenExtent;
}

VkSurfaceFormatKHR Swapchain::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
    for (const auto& availableFormat : availableFormats)
    {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            return availableFormat;
    }

    return availableFormats[0];
}

VkPresentModeKHR Swapchain::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
    for (const auto& availablePresentMode : availablePresentModes)
    {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
            return availablePresentMode;
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D Swapchain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, VkExtent2D extent)
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        return capabilities.currentExtent;
    }

    VkExtent2D actualExtent = {
            extent.width,
            extent.height
    };

    actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

    return actualExtent;
}

void Swapchain::Recreate(VkExtent2D newExtent)
{
    CleanupSwapchain();
    CreateSwapchain(newExtent);
    CreateImageViews();
}

void Swapchain::CleanupSwapchain()
{
    for (auto& swapChainImageView : m_ImageViews) {
        vkDestroyImageView(m_Device->Get(), swapChainImageView, nullptr);
    }

    vkDestroySwapchainKHR(m_Device->Get(), m_Swapchain, nullptr);
}

void Swapchain::CreateImageViews()
{
    m_ImageViews.resize(m_Images.size());

    for (size_t i = 0; i < m_Images.size(); i++)
    {
        VkImageViewCreateInfo createInfo { };
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = m_Images[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = m_SwapChainFormat;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(m_Device->Get(), &createInfo, nullptr, &m_ImageViews[i]) != VK_SUCCESS)
            throw std::runtime_error("failed to create image views!");
    }
}

void Swapchain::CreateSyncObjects()
{
    m_ImageAvailableSemaphores.resize(FramesInFlight);
    m_RenderFinishedSemaphores.resize(FramesInFlight);
    m_InFlightFences.resize(FramesInFlight);

    VkSemaphoreCreateInfo semaphoreInfo { };
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo { };
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < FramesInFlight; i++)
    {
        if (vkCreateSemaphore(m_Device->Get(), &semaphoreInfo, nullptr, &m_ImageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(m_Device->Get(), &semaphoreInfo, nullptr, &m_RenderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(m_Device->Get(), &fenceInfo, nullptr, &m_InFlightFences[i]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create synchronization objects for a frame!");
        }
    }
}

VkResult Swapchain::AcquireNextImage(uint32_t* imageIndex)
{
    vkWaitForFences(m_Device->Get(), 1, &m_InFlightFences[m_FramesIndex], VK_TRUE, UINT64_MAX);
    return vkAcquireNextImageKHR(m_Device->Get(), m_Swapchain, UINT64_MAX, m_ImageAvailableSemaphores[m_FramesIndex], VK_NULL_HANDLE, imageIndex);
}

void Swapchain::SubmitCommandBuffer(VkCommandBuffer commandBuffer)
{
    VkSubmitInfo submitInfo {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {m_ImageAvailableSemaphores[m_FramesIndex]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    std::array<VkCommandBuffer, 1> commandBuffers = {commandBuffer};

    submitInfo.commandBufferCount = commandBuffers.size();
    submitInfo.pCommandBuffers = commandBuffers.data();

    VkSemaphore signalSemaphores[] = {m_RenderFinishedSemaphores[m_FramesIndex]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(m_Device->GetGraphicsQueue(), 1, &submitInfo, m_InFlightFences[m_FramesIndex]) != VK_SUCCESS)
        throw std::runtime_error("Failed to submit draw command buffer!");
}

VkResult Swapchain::Present()
{
    VkSemaphore signalSemaphores[] = {m_RenderFinishedSemaphores[m_FramesIndex]};
    VkPresentInfoKHR presentInfo { };
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {m_Swapchain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &m_FramesIndex;

    m_FramesIndex = (m_FramesIndex + 1) % FramesInFlight;

    return vkQueuePresentKHR(m_Device->GetPresentQueue(), &presentInfo);;
}

void Swapchain::ResetFence()
{
    vkResetFences(m_Device->Get(), 1, &m_InFlightFences[m_FramesIndex]);
}
