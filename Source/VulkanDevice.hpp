#pragma once

#include "vulkan/vulkan.h"

#include <vector>
#include <memory>
#include <optional>

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
    inline VkPhysicalDevice GetPhysicalDevice() const { return m_PhysicalDevice; }
    inline QueueFamilyIndices GetQueueFamilyIndices() const { return m_QueueFamilyIndices; }
    SwapChainSupportDetails GetSwapChainSupportDetails();
private:
    VkInstance m_Instance = VK_NULL_HANDLE;
    VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
    VkQueue m_GraphicsQueue = VK_NULL_HANDLE;
    VkQueue m_PresentQueue = VK_NULL_HANDLE;
    QueueFamilyIndices m_QueueFamilyIndices;

    bool m_EnableValidationLayers;

    std::vector<VkPhysicalDevice> m_AvailablePhysicalDevices;
    VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
    VkDevice m_Device = VK_NULL_HANDLE;

    void FindPhysicalDevices();
    void PickPhysicalDevice();
    void CreateLogicalDevice();
    bool IsDeviceSuitable(VkPhysicalDevice device);

    static bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
    static QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
    static SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
};
