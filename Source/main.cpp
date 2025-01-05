#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "Assert.hpp"

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <filesystem>
#include <cstring>
#include <optional>

#define print(x) std::cout << x << '\n'

#ifdef NDEBUG
    const bool EnableValidationLayers = false;
#else
    const bool EnableValidationLayers = true;
#endif

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

bool CheckValidationLayerSupport()
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : validationLayers)
    {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers)
        {
            if (strcmp(layerName, layerProperties.layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }

        if (!layerFound)
        {
            return false;
        }
    }

    return true;
}

struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily;

    bool IsComplete() const
    {
        return graphicsFamily.has_value();
    }
};

class VulkanApp // NOLINT(*-pro-type-member-init)
{
public:
    void Run()
    {
        InitWindow();
        InitVulkan();
        MainLoop();
        Cleanup();
    }
private:
    void InitWindow()
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        m_Window = glfwCreateWindow((int)WINDOW_WIDTH, (int)WINDOW_HEIGHT,
                                    "Learn Vulkan", nullptr, nullptr);
    }

    void CreateInstance()
    {
        // Print all available extensions, comment out if not needed
        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        auto supportedExtensions = std::vector<VkExtensionProperties>(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, supportedExtensions.data());
        printf("Extensions available: %u\n", extensionCount);
        for (const auto& ext : supportedExtensions)
           printf("\t %s \n", ext.extensionName);

        if (EnableValidationLayers && !CheckValidationLayerSupport())
        {
            throw std::runtime_error("Validation layers requested, but not available!");
        }

        VkApplicationInfo appInfo { };
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Learn Vulkan";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_3;

        VkInstanceCreateInfo createInfo { };
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        // Acquire extensions required for window to work
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        createInfo.enabledExtensionCount = glfwExtensionCount;
        createInfo.ppEnabledExtensionNames = glfwExtensions;

        // Validation layers enabling
        if (EnableValidationLayers)
        {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        }
        else
            createInfo.enabledLayerCount = 0;

        auto result = vkCreateInstance(&createInfo, nullptr, &m_Instance);
        if (result != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create Vulkan instance!");
        }
    }

    void PickPhysicalDevice()
    {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr);

        if (deviceCount == 0)
        {
            throw std::runtime_error("Failed to find GPUs with Vulkan support!");
        }

        auto devices = std::vector<VkPhysicalDevice>(deviceCount);
        vkEnumeratePhysicalDevices(m_Instance, &deviceCount, devices.data());

        // Pick the first device that supports graphics queue
        for (const auto& device : devices)
        {
            if (IsDeviceSuitable(device))
            {
                m_PhysicalDevice = device;
                break;
            }
        }

        if (m_PhysicalDevice == VK_NULL_HANDLE)
        {
            throw std::runtime_error("Failed to find a suitable GPU!");
        }
    }

    bool IsDeviceSuitable(VkPhysicalDevice device)
    {
        auto indices = FindQueueFamilies(device);
        return indices.IsComplete();
    }

    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device)
    {
        // Find all queue families that are available on this device

        QueueFamilyIndices indices;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());


        // Check of graphics queue family is supported
        int i = 0;
        for (const auto& queueFamily : queueFamilies)
        {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
                indices.graphicsFamily = i;

            if (indices.IsComplete())
                break;

            i++;
        }

        return indices;
    }

    void CreateLogicalDevice()
    {
        QueueFamilyIndices indices = FindQueueFamilies(m_PhysicalDevice);

        VkDeviceQueueCreateInfo queueCreateInfo { };
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
        queueCreateInfo.queueCount = 1;
        float queuePriority = 1.0f;
        queueCreateInfo.pQueuePriorities = &queuePriority;

        // Empty for now
        VkPhysicalDeviceFeatures deviceFeatures { };

        VkDeviceCreateInfo createInfo { };
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pQueueCreateInfos = &queueCreateInfo;
        createInfo.queueCreateInfoCount = 1;
        createInfo.pEnabledFeatures = &deviceFeatures;

        createInfo.enabledExtensionCount = 0;

        if (EnableValidationLayers)
        {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        } else
        {
            createInfo.enabledLayerCount = 0;
        }

        if (vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_Device) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create logical device!");
        }

        vkGetDeviceQueue(m_Device, indices.graphicsFamily.value(), 0, &m_GraphicsQueue);
    }

    void InitVulkan()
    {
        CreateInstance();
        PickPhysicalDevice();
        CreateLogicalDevice();
    }

    void MainLoop()
    {
        while (!glfwWindowShouldClose(m_Window))
        {
            glfwPollEvents();
        }
    }

    void Cleanup()
    {
        vkDestroyDevice(m_Device, nullptr);
        vkDestroyInstance(m_Instance, nullptr);
        glfwDestroyWindow(m_Window);
        glfwTerminate();
    }
private:
    const uint32_t WINDOW_WIDTH = 1280;
    const uint32_t WINDOW_HEIGHT = 720;

    GLFWwindow* m_Window;

    VkInstance m_Instance;
    VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
    VkDevice m_Device;
    VkQueue m_GraphicsQueue;
};

int32_t main(int32_t argc, char* argv[])
{
    auto app = VulkanApp();

    try
    {
        app.Run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

