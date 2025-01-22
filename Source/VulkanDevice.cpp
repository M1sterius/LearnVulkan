#include "VulkanDevice.hpp"
#include "VulkanInstance.hpp"
#include "GlobalUtility.hpp"

#include <set>

VulkanDevice::VulkanDevice(VkInstance& instance, VkSurfaceKHR& surface)
    :   m_Instance(instance), m_Surface(surface)
{
    FindPhysicalDevices();
    PickPhysicalDevice();
}

VulkanDevice::~VulkanDevice()
{

}

void VulkanDevice::FindPhysicalDevices()
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr);

    if (deviceCount == 0)
        throw std::runtime_error("Failed to find GPUs with Vulkan support!");

    m_AvailablePhysicalDevices.resize(deviceCount);
    vkEnumeratePhysicalDevices(m_Instance, &deviceCount, m_AvailablePhysicalDevices.data());
}

bool VulkanDevice::IsDeviceSuitable(VkPhysicalDevice device)
{
    return false;
}

void VulkanDevice::PickPhysicalDevice()
{

}

bool VulkanDevice::CheckDeviceExtensionSupport(VkPhysicalDevice device)
{
    // Check if all required extensions are supported by device

    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    for (const auto& extension : availableExtensions)
    {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}
