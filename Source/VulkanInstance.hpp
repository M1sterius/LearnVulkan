#pragma once

#include "vulkan/vulkan.h"

#include <vector>

class VulkanInstance
{
public:
    explicit VulkanInstance(const char* appName, bool enableValidationLayers = true);
    ~VulkanInstance();

    VulkanInstance(const VulkanInstance&) = delete;
    VulkanInstance operator = (const VulkanInstance&) = delete;

    void SetupDebugMessenger();
    void DestroyDebugMessenger();

    inline bool GetValidationLayersEnabled() const { return m_EnableValidationLayers; }
    inline VkInstance Get() const { return m_Instance; }
private:
    bool m_EnableValidationLayers;
    std::vector<VkExtensionProperties> m_SupportedExtensions;
    VkInstance m_Instance = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT m_DebugMessenger = VK_NULL_HANDLE;

    static bool CheckValidationLayerSupport();
    static void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
};
