#pragma once

#include "vulkan/vulkan.h"

#include <vector>
#include <memory>
#include <optional>

// Device extensions that will be enabled
static const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

struct QueueFamilyIndices
{
    // If a device does not support a queue family, the corresponding std::optional won't have a value

    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool IsComplete() const
    {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities { };
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

class VulkanDevice
{
public:
    VulkanDevice(VkInstance instance, VkSurfaceKHR surface, bool enableValidationLayers);
    ~VulkanDevice();

    VulkanDevice(const VulkanDevice&) = delete;
    VulkanDevice operator = (const VulkanDevice&) = delete;

    inline VkQueue GetGraphicsQueue() const { return m_GraphicsQueue; }
    inline VkQueue GetPresentQueue() const { return m_PresentQueue; }
    inline VkDevice Get() const { return m_Device; }
    inline VkCommandPool GetCommandPool() const { return m_CommandPool; }
    inline VkPhysicalDevice GetPhysicalDevice() const { return m_PhysicalDevice; }
    inline VkPhysicalDeviceProperties GetPhysicalDeviceProperties() const { return m_PhysicalDeviceProperties; }
    inline QueueFamilyIndices GetQueueFamilyIndices() const { return m_QueueFamilyIndices; }
    SwapChainSupportDetails GetSwapChainSupportDetails();
    uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

    VkCommandBuffer BeginSingleTimeCommands();
    void EndSingleTimeCommands(VkCommandBuffer commandBuffer);
    void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
    void MapMemory(VkDeviceMemory memory, VkDeviceSize size, VkDeviceSize offset, VkMemoryMapFlags flags, void* data);
    void CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling,
                     VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image,
                     VkDeviceMemory& imageMemory);
    void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
    void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
private:
    VkInstance m_Instance = VK_NULL_HANDLE;
    VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
    VkQueue m_GraphicsQueue = VK_NULL_HANDLE;
    VkQueue m_PresentQueue = VK_NULL_HANDLE;
    VkCommandPool m_CommandPool = VK_NULL_HANDLE;
    VkPhysicalDeviceProperties m_PhysicalDeviceProperties;
    QueueFamilyIndices m_QueueFamilyIndices;

    std::vector<VkPhysicalDevice> m_AvailablePhysicalDevices;
    VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
    VkDevice m_Device = VK_NULL_HANDLE;

    bool m_EnableValidationLayers;

    void FindPhysicalDevices();
    void PickPhysicalDevice();
    void CreateLogicalDevice();
    bool IsDeviceSuitable(VkPhysicalDevice device);
    void CreateCommandPool();

    static bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
    static QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
    static SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
};
