#pragma once

#include "Device.hpp"

#include <filesystem>
#include <vector>

namespace lv
{
    struct PipelineConfigInfo
    {
        VkViewport viewport;
        VkRect2D scissor;
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
        VkPipelineRasterizationStateCreateInfo rasterizationInfo;
        VkPipelineMultisampleStateCreateInfo multisampleInfo;
        VkPipelineColorBlendAttachmentState colorBlendAttachment;
        VkPipelineColorBlendStateCreateInfo colorBlendInfo;
        VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
        VkPipelineLayout pipelineLayout = nullptr;
        VkRenderPass renderPass = nullptr;
        uint32_t subpass = 0;
    };

    class Pipeline
    {
    public:
        Pipeline(Device& device, const PipelineConfigInfo& pipelineConfigInfo,
            const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath);
        ~Pipeline();

        Pipeline(const Pipeline&) = delete;
        Pipeline operator = (const Pipeline&) = delete;

        void Bind(VkCommandBuffer commandBuffer);

        static PipelineConfigInfo DefaultPipelineConfigInfo(uint32_t width, uint32_t height);
    private:
        static std::vector<char> ReadFile(const std::filesystem::path& path);
        void CreateGraphicsPipeline(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath,
                            const PipelineConfigInfo& pipelineConfigInfo);

        void CreateShaderModule(const std::vector<char>& source, VkShaderModule* shaderModule);

        Device& m_Device;
        VkPipeline m_GraphicsPipeline;
        VkShaderModule m_VertShaderModule;
        VkShaderModule m_FragShaderModule;
    };
}
