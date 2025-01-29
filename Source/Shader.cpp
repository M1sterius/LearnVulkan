#include "Shader.hpp"
#include "VulkanDevice.hpp"

#include <fstream>

Shader::Shader(VulkanDevice* device, const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath)
    :   m_Device(device)
{

    const auto vertByteCode = ReadFile(vertexPath);
    const auto fragByteCode = ReadFile(fragmentPath);

    VkShaderModuleCreateInfo vertCreateInfo { };
    vertCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    vertCreateInfo.codeSize = vertByteCode.size();
    vertCreateInfo.pCode = reinterpret_cast<const uint32_t*>(vertByteCode.data());

    VkShaderModuleCreateInfo fragCreateInfo { };
    fragCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    fragCreateInfo.codeSize = fragByteCode.size();
    fragCreateInfo.pCode = reinterpret_cast<const uint32_t*>(fragByteCode.data());

    if (vkCreateShaderModule(m_Device->Get(), &vertCreateInfo, nullptr, &m_VertexModule) != VK_SUCCESS)
        throw std::runtime_error("Failed to create vertex shader module!");
    if (vkCreateShaderModule(m_Device->Get(), &fragCreateInfo, nullptr, &m_FragmentModule) != VK_SUCCESS)
        throw std::runtime_error("Failed to create fragment shader module!");

    CreateStagesInfo();
}

Shader::~Shader()
{
    vkDestroyShaderModule(m_Device->Get(), m_VertexModule, nullptr);
    vkDestroyShaderModule(m_Device->Get(), m_FragmentModule, nullptr);
}

void Shader::CreateStagesInfo()
{
    m_ShaderStagesInfo[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    m_ShaderStagesInfo[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    m_ShaderStagesInfo[0].module = m_VertexModule;
    m_ShaderStagesInfo[0].pName = "main";

    m_ShaderStagesInfo[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    m_ShaderStagesInfo[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    m_ShaderStagesInfo[1].module = m_FragmentModule;
    m_ShaderStagesInfo[1].pName = "main";
}

std::vector<char> Shader::ReadFile(const std::filesystem::path& path)
{
    auto file = std::ifstream(path, std::ios::ate | std::ios::binary);

    if (!file.is_open())
        throw std::runtime_error("Failed to open file: " + path.string());

    const auto fileSize = static_cast<size_t>(file.tellg());
    auto buffer = std::vector<char>(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}
