#pragma once

#include "vulkan/vulkan.h"

#include <vector>

class VulkanInstance
{
public:
    explicit VulkanInstance(const char* appName, bool enableValidationLayers);
    ~VulkanInstance();

    VulkanInstance(const VulkanInstance&) = delete;
    VulkanInstance operator = (const VulkanInstance&) = delete;

    inline std::vector<VkExtensionProperties> GetSupportedExtensions() const { return m_SupportedExtensions; }
    inline bool GetValidationLayersEnabled() const { return m_EnableValidationLayers; }
    inline VkInstance Get() const { return m_Instance; }
private:
    bool m_EnableValidationLayers;
    std::vector<VkExtensionProperties> m_SupportedExtensions;
    VkInstance m_Instance = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT m_DebugMessenger = VK_NULL_HANDLE;

    void SetupDebugMessenger();
    void DestroyDebugMessenger();

    static bool CheckValidationLayerSupport();
    static void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
};
