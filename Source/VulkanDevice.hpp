#pragma once

#include "vulkan/vulkan.h"

#include <vector>
#include <memory>

class VulkanInstance;

class VulkanDevice
{
public:
    explicit VulkanDevice(VkInstance& instance, VkSurfaceKHR& surface, bool enableValidationLayers);
    ~VulkanDevice();

    VulkanDevice(const VulkanDevice&) = delete;
    VulkanDevice operator = (const VulkanDevice&) = delete;

    inline VkQueue GetGraphicsQueue() const { return m_GraphicsQueue; }
    inline VkQueue GetPresentQueue() const { return m_PresentQueue; }
    inline VkDevice Get() const { return m_Device; }
    inline VkPhysicalDevice GetPhysicalDevice() const { return m_PhysicalDevice; }
private:
    VkInstance& m_Instance;
    VkSurfaceKHR& m_Surface;
    VkQueue m_GraphicsQueue = VK_NULL_HANDLE;
    VkQueue m_PresentQueue = VK_NULL_HANDLE;
    bool m_EnableValidationLayers;

    std::vector<VkPhysicalDevice> m_AvailablePhysicalDevices;
    VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
    VkDevice m_Device = VK_NULL_HANDLE;

    void FindPhysicalDevices();
    void PickPhysicalDevice();
    void CreateLogicalDevice();
    bool IsDeviceSuitable(VkPhysicalDevice device);

    static bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
};
