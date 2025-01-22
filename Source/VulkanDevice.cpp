#include "VulkanDevice.hpp"
#include "VulkanInstance.hpp"
#include "GlobalUtility.hpp"

#include <set>

VulkanDevice::VulkanDevice(VkInstance instance, VkSurfaceKHR surface, bool enableValidationLayers)
    :   m_Instance(instance), m_Surface(surface), m_EnableValidationLayers(enableValidationLayers)
{
    FindPhysicalDevices();
    PickPhysicalDevice();
    CreateLogicalDevice();
}

VulkanDevice::~VulkanDevice()
{
    vkDestroyDevice(m_Device, nullptr);
}

void VulkanDevice::FindPhysicalDevices()
{
    // Search for all physical GPUs on this PC

    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr);

    if (deviceCount == 0)
        throw std::runtime_error("Failed to find GPUs with Vulkan support!");

    m_AvailablePhysicalDevices.resize(deviceCount);
    vkEnumeratePhysicalDevices(m_Instance, &deviceCount, m_AvailablePhysicalDevices.data());
}

bool VulkanDevice::IsDeviceSuitable(VkPhysicalDevice device)
{
    // Check if physical device supports all required extensions, queue families and has sufficient swapchain support

    const auto indices = FindQueueFamilies(device, m_Surface);
    const auto areExtensionsSupported = CheckDeviceExtensionSupport(device);

    bool isSwapChainAdequate = false;
    if (areExtensionsSupported)
    {
        const auto swapChainSupport = QuerySwapChainSupport(device, m_Surface);
        isSwapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }

    return indices.IsComplete() && areExtensionsSupported && isSwapChainAdequate;
}

void VulkanDevice::PickPhysicalDevice()
{
    // Pick the first suitable device
    for (const auto& device : m_AvailablePhysicalDevices)
    {
        if (IsDeviceSuitable(device))
        {
            m_PhysicalDevice = device;
            m_QueueFamilyIndices = FindQueueFamilies(device, m_Surface);
            break;
        }
    }

    if (m_PhysicalDevice == VK_NULL_HANDLE)
        throw std::runtime_error("Failed to find a suitable GPU!");
}

void VulkanDevice::CreateLogicalDevice()
{
    const auto indices = FindQueueFamilies(m_PhysicalDevice, m_Surface);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    // Empty for now
    VkPhysicalDeviceFeatures deviceFeatures { };

    VkDeviceCreateInfo createInfo { };
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    if (m_EnableValidationLayers)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else
    {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_Device) != VK_SUCCESS)
        throw std::runtime_error("Failed to create logical device!");

    vkGetDeviceQueue(m_Device, indices.graphicsFamily.value(), 0, &m_GraphicsQueue);
    vkGetDeviceQueue(m_Device, indices.presentFamily.value(), 0, &m_PresentQueue);
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

QueueFamilyIndices VulkanDevice::FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    // Searches for support of graphics and present queue families on device,
    // corresponding std::optional in QueueFamilyIndices won't have a values if the queue isn't supported

    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto& queueFamily : queueFamilies)
    {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            indices.graphicsFamily = i;

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

        if (presentSupport)
            indices.presentFamily = i;

        if (indices.IsComplete())
            break;

        i++;
    }

    return indices;
}

SwapChainSupportDetails VulkanDevice::QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    // Retrieve information about swap chain support on device

    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

    // Surface format capabilities
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
    if (formatCount != 0)
    {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
    }

    // Surface present modes
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
    if (presentModeCount != 0)
    {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}

SwapChainSupportDetails VulkanDevice::GetSwapChainSupportDetails()
{
    return QuerySwapChainSupport(m_PhysicalDevice, m_Surface);
}
