#pragma once

#include <vulkan/vulkan.h>

#include <filesystem>
#include <vector>
#include <array>

class VulkanDevice;

class Shader
{
public:
    Shader(VulkanDevice* device, const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath);
    ~Shader();

    Shader(const Shader&) = delete;
    Shader operator = (const Shader&) = delete;

    inline VkShaderModule GetVertexModule() const { return m_VertexModule; }
    inline VkShaderModule GetFragmentModule() const { return m_FragmentModule; }
    inline std::array<VkPipelineShaderStageCreateInfo, 2> GetShaderStagesInfo() const { return m_ShaderStagesInfo; }
private:
    VulkanDevice* m_Device;
    VkShaderModule m_VertexModule = VK_NULL_HANDLE;
    VkShaderModule m_FragmentModule = VK_NULL_HANDLE;
    std::array<VkPipelineShaderStageCreateInfo, 2> m_ShaderStagesInfo { };

    void CreateStagesInfo();

    static std::vector<char> ReadFile(const std::filesystem::path& path);
};
