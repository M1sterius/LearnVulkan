#pragma once

#include "vulkan/vulkan.h"

#include <vector>
#include <memory>

class VulkanInstance;

class VulkanDevice
{
public:
    explicit VulkanDevice(VkInstance& instance, VkSurfaceKHR& surface);
    ~VulkanDevice();

    VulkanDevice(const VulkanDevice&) = delete;
    VulkanDevice operator = (const VulkanDevice&) = delete;
private:
    VkInstance& m_Instance;
    VkSurfaceKHR& m_Surface;

    std::vector<VkPhysicalDevice> m_AvailablePhysicalDevices;
    VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
    VkDevice m_Device = VK_NULL_HANDLE;

    void FindPhysicalDevices();
    void PickPhysicalDevice();
    bool IsDeviceSuitable(VkPhysicalDevice device);

    static bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
};
