#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"

#include "Assert.hpp"
#include "Window.hpp"
#include "VulkanInstance.hpp"
#include "VulkanDevice.hpp"
#include "Swapchain.hpp"
#include "VertexBuffer.hpp"
#include "IndexBuffer.hpp"
#include "Texture.hpp"
#include "Shader.hpp"
#include "Framebuffer.hpp"
#include "CommandBuffer.hpp"

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <filesystem>
#include <cstring>
#include <optional>
#include <set>
#include <algorithm>
#include <fstream>
#include <array>

#define print(x) std::cout << x << '\n'

#ifdef NDEBUG
    constexpr bool EnableValidationLayers = false;
#else
    constexpr bool EnableValidationLayers = true;
#endif

const std::vector<Vertex> vertices = {
    {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
    {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
    {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},

    {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
    {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
    {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
};

const std::vector<uint32_t> indices = {
    0, 1, 2, 2, 3, 0,
    4, 5, 6, 6, 7, 4
};

struct UBO
{
    glm::mat4 MVP;
};

class VulkanApp
{
public:
    void Run()
    {
        m_Window = std::make_unique<Window>(800, 600, "Learn Vulkan!");
        InitVulkan();
        MainLoop();
        Cleanup();
    }
private:
    void OnSwapchainRecreateRequired()
    {
        m_Window->WaitForMinimize(); // Wait until the window is no longer minimized
        vkDeviceWaitIdle(m_Device->Get());

        vkDestroyImageView(m_Device->Get(), m_DepthImageView, nullptr);
        vkDestroyImage(m_Device->Get(), m_DepthImage, nullptr);
        vkFreeMemory(m_Device->Get(), m_DepthImageMemory, nullptr);

        m_Swapchain->Recreate(m_Window->GetExtent());
        CreateDepthResources();
        RecreateFramebuffers();
    }

    void CreateRenderPass()
    {
        VkAttachmentDescription colorAttachment { };
        colorAttachment.format = m_Swapchain->GetFormat();
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentDescription depthAttachment {};
        depthAttachment.format = VK_FORMAT_D32_SFLOAT;
        depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        
        VkAttachmentReference colorAttachmentRef {};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depthAttachmentRef {};
        depthAttachmentRef.attachment = 1;
        depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;
        subpass.pDepthStencilAttachment = &depthAttachmentRef;

        VkSubpassDependency dependency {};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        std::array<VkAttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};

        VkRenderPassCreateInfo renderPassInfo {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = attachments.size();
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        if (vkCreateRenderPass(m_Device->Get(), &renderPassInfo, nullptr, &m_RenderPass) != VK_SUCCESS)
            throw std::runtime_error("Failed to create render pass!");
    }

    void CreateDescriptorSetLayout()
    {
        VkDescriptorSetLayoutBinding uboLayoutBinding {};
        uboLayoutBinding.binding = 0;
        uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.descriptorCount = 1;
        uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        VkDescriptorSetLayoutBinding samplerLayoutBinding {};
        samplerLayoutBinding.binding = 1;
        samplerLayoutBinding.descriptorCount = 1;
        samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };

        VkDescriptorSetLayoutCreateInfo layoutInfo {};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = bindings.size();
        layoutInfo.pBindings = bindings.data();

        if (vkCreateDescriptorSetLayout(m_Device->Get(), &layoutInfo, nullptr, &m_DescriptorSetLayout) != VK_SUCCESS)
            throw std::runtime_error("Failed to create descriptor set layout!");
    }

    void CreateUniformBuffers()
    {
        VkDeviceSize bufferSize = sizeof(UBO);

        m_UniformBuffers.resize(Swapchain::FramesInFlight);
        m_UniformBuffersMemory.resize(Swapchain::FramesInFlight);
        m_UniformBuffersMapData.resize(Swapchain::FramesInFlight);

        for (size_t i = 0; i < Swapchain::FramesInFlight; i++)
        {
            m_Device->CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_UniformBuffers[i], m_UniformBuffersMemory[i]);

            vkMapMemory(m_Device->Get(), m_UniformBuffersMemory[i], 0, bufferSize, 0, &m_UniformBuffersMapData[i]);
        }
    }

    void UpdateUniformBuffer(uint32_t currentFrame)
    {
        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

        UBO ubo {};
        auto model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        auto view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        auto proj = glm::perspective(glm::radians(45.0f), (float)m_Swapchain->GetExtent().width / (float)m_Swapchain->GetExtent().height, 0.1f, 10.0f);
        proj[1][1] *= -1;

        ubo.MVP = proj * view * model;

        memcpy(m_UniformBuffersMapData[currentFrame], &ubo, sizeof(ubo));
    }

    void CreateDescriptorPool()
    {
        std::array<VkDescriptorPoolSize, 2> poolSizes {};

        poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[0].descriptorCount = Swapchain::FramesInFlight;

        poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[1].descriptorCount = Swapchain::FramesInFlight;

        VkDescriptorPoolCreateInfo poolInfo {};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = 1;
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = Swapchain::FramesInFlight;

        if (vkCreateDescriptorPool(m_Device->Get(), &poolInfo, nullptr, &m_DescriptorPool) != VK_SUCCESS)
            throw std::runtime_error("failed to create descriptor pool!");
    }

    void CreateDescriptorSets()
    {
        std::vector<VkDescriptorSetLayout> layouts(Swapchain::FramesInFlight, m_DescriptorSetLayout);

        VkDescriptorSetAllocateInfo allocInfo {};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = m_DescriptorPool;
        allocInfo.descriptorSetCount = Swapchain::FramesInFlight;
        allocInfo.pSetLayouts = layouts.data();

        m_DescriptorSets.resize(Swapchain::FramesInFlight);

        if (vkAllocateDescriptorSets(m_Device->Get(), &allocInfo, m_DescriptorSets.data()) != VK_SUCCESS)
            throw std::runtime_error("Failed to allocate descriptor sets!");

        for (size_t i = 0; i < Swapchain::FramesInFlight; i++)
        {
            VkDescriptorBufferInfo bufferInfo {};
            bufferInfo.buffer = m_UniformBuffers[i];
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(UBO);

            VkDescriptorImageInfo imageInfo {};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = m_Texture->GetImageView();
            imageInfo.sampler = m_TextureSampler;

            std::array<VkWriteDescriptorSet, 2> descriptorWrites {};

            descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[0].dstSet = m_DescriptorSets[i];
            descriptorWrites[0].dstBinding = 0;
            descriptorWrites[0].dstArrayElement = 0;
            descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[0].descriptorCount = 1;
            descriptorWrites[0].pBufferInfo = &bufferInfo;

            descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[1].dstSet = m_DescriptorSets[i];
            descriptorWrites[1].dstBinding = 1;
            descriptorWrites[1].dstArrayElement = 0;
            descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[1].descriptorCount = 1;
            descriptorWrites[1].pImageInfo = &imageInfo;

            vkUpdateDescriptorSets(m_Device->Get(), descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
        }
    }

    void CreateGraphicsPipeline()
    {
        const auto shader = std::make_unique<Shader>(m_Device.get(), "Assets/Shaders/simple_shader.vert.spv", "Assets/Shaders/simple_shader.frag.spv");
        const auto shaderStagesInfo = shader->GetShaderStagesInfo();

        // How vertices will be imported from vertex buffer
        VkPipelineVertexInputStateCreateInfo vertexInputInfo { };

        auto bindingDescription = Vertex::GetBindingDescription();
        auto attributeDescriptions = Vertex::GetAttributeDescriptions();

        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());;
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

        // How vertices will be assembled into primitives
        VkPipelineInputAssemblyStateCreateInfo inputAssembly { };
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        // Enabling dynamic viewport and scissor
        VkPipelineViewportStateCreateInfo viewportState {};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;

        // Rasterization config
        VkPipelineRasterizationStateCreateInfo rasterizer { };
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;
        rasterizer.depthBiasConstantFactor = 0.0f; // Optional
        rasterizer.depthBiasClamp = 0.0f; // Optional
        rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

        // Multisampling config
        VkPipelineMultisampleStateCreateInfo multisampling { };
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampling.minSampleShading = 1.0f; // Optional
        multisampling.pSampleMask = nullptr; // Optional
        multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
        multisampling.alphaToOneEnable = VK_FALSE; // Optional

        // Depth stencil
        VkPipelineDepthStencilStateCreateInfo depthStencil {};
        depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencil.depthTestEnable = VK_TRUE;
        depthStencil.depthWriteEnable = VK_TRUE;
        depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
        depthStencil.depthBoundsTestEnable = VK_FALSE;
        depthStencil.stencilTestEnable = VK_FALSE;

        // Color blending config per framebuffer
        VkPipelineColorBlendAttachmentState colorBlendAttachment { };
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

        // Global color blending config
        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;

        std::vector<VkDynamicState> dynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };
        VkPipelineDynamicStateCreateInfo dynamicState { };
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicState.pDynamicStates = dynamicStates.data();

        VkPipelineLayoutCreateInfo pipelineLayoutInfo { };
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &m_DescriptorSetLayout;
        pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
        pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

        if (vkCreatePipelineLayout(m_Device->Get(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
            throw std::runtime_error("failed to create pipeline layout!");

        VkGraphicsPipelineCreateInfo pipelineInfo {};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = shaderStagesInfo.size();
        pipelineInfo.pStages = shaderStagesInfo.data();
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pDepthStencilState = &depthStencil;
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDynamicState = &dynamicState;
        pipelineInfo.layout = m_PipelineLayout;
        pipelineInfo.renderPass = m_RenderPass;
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
        pipelineInfo.basePipelineIndex = -1; // Optional

        if (vkCreateGraphicsPipelines(m_Device->Get(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_Pipeline) != VK_SUCCESS)
            throw std::runtime_error("Failed to create graphics pipeline!");
    }

    void CreateTextureSampler()
    {
        VkSamplerCreateInfo samplerInfo {};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.maxAnisotropy = m_Device->GetPhysicalDeviceProperties().limits.maxSamplerAnisotropy;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

        if (vkCreateSampler(m_Device->Get(), &samplerInfo, nullptr, &m_TextureSampler) != VK_SUCCESS)
            throw std::runtime_error("Failed to create texture sampler!");
    }

    VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
    {
        VkImageViewCreateInfo viewInfo {};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = format;
        viewInfo.subresourceRange.aspectMask = aspectFlags;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        VkImageView imageView;

        if (vkCreateImageView(m_Device->Get(), &viewInfo, nullptr, &imageView) != VK_SUCCESS)
            throw std::runtime_error("failed to create texture image view!");

        return imageView;
    }

    void CreateFramebuffers()
    {
        for (size_t i = 0; i < m_Swapchain->GetImageViews().size(); i++)
        {
            const std::vector<VkImageView> attachments = {m_Swapchain->GetImageViews()[i], m_DepthImageView};
            m_Framebuffers.emplace_back(std::make_unique<Framebuffer>(m_Device.get(), m_RenderPass, attachments, m_Swapchain->GetExtent()));
        }
    }

    void CreateDepthResources()
    {
        const auto depthFormat = VK_FORMAT_D32_SFLOAT; // 32-bit float depth without stencil component

        m_Device->CreateImage(m_Swapchain->GetExtent().width, m_Swapchain->GetExtent().height,
              depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
              VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_DepthImage, m_DepthImageMemory);

        m_DepthImageView = CreateImageView(m_DepthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
    }

    void RecreateFramebuffers()
    {
        // Cleanup old framebuffers
        for (auto& swapChainFramebuffer : m_Framebuffers)
            swapChainFramebuffer.reset();
        m_Framebuffers.clear(); // make sure that there are no traces of old framebuffers in the vector

        CreateFramebuffers();
    }

    void CreateCommandBuffers()
    {
        for (size_t i = 0; i < Swapchain::FramesInFlight; i++)
        {
            m_CommandBuffers.emplace_back(std::make_unique<CommandBuffer>(m_Device.get()));
        }
    }

    void RecordCommandBuffer(const std::unique_ptr<CommandBuffer>& commandBuffer, const uint32_t imageIndex)
    {
        commandBuffer->BeginRecording(0);

        VkRenderPassBeginInfo renderPassInfo { };
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = m_RenderPass;
        renderPassInfo.framebuffer = m_Framebuffers[imageIndex]->Get();
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = m_Swapchain->GetExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
        clearValues[1].depthStencil = {1.0f, 0};

        renderPassInfo.clearValueCount = clearValues.size();
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer->Get(), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(commandBuffer->Get(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline);

        VkViewport viewport {};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(m_Swapchain->GetExtent().width);
        viewport.height = static_cast<float>(m_Swapchain->GetExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(commandBuffer->Get(), 0, 1, &viewport);

        VkRect2D scissor {};
        scissor.offset = {0, 0};
        scissor.extent = m_Swapchain->GetExtent();
        vkCmdSetScissor(commandBuffer->Get(), 0, 1, &scissor);

        VkBuffer vertexBuffers[] = {m_VertexBuffer->Get()};
        VkDeviceSize offsets[] = {0};

        vkCmdBindVertexBuffers(commandBuffer->Get(), 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(commandBuffer->Get(), m_IndexBuffer->Get(), 0, VK_INDEX_TYPE_UINT32);

        vkCmdBindDescriptorSets(commandBuffer->Get(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, 1, &m_DescriptorSets[m_Swapchain->GetCurrentFrameIndex()], 0, nullptr);

        vkCmdDrawIndexed(commandBuffer->Get(), m_IndexBuffer->GetIndicesCount(), 1, 0, 0, 0);

        vkCmdEndRenderPass(commandBuffer->Get());

        commandBuffer->EndRecording();
    }

    void InitVulkan()
    {
        m_Instance = std::make_unique<VulkanInstance>("Vulkan App", EnableValidationLayers);
        m_Surface = m_Window->CreateWindowSurface(m_Instance->Get());
        m_Device = std::make_unique<VulkanDevice>(m_Instance->Get(), m_Surface, EnableValidationLayers);
        m_Swapchain = std::make_unique<Swapchain>(m_Device.get(), m_Surface, m_Window->GetExtent());
        CreateDepthResources();
        CreateRenderPass();
        CreateFramebuffers();
        CreateDescriptorSetLayout();
        CreateGraphicsPipeline();
        m_VertexBuffer = std::make_unique<VertexBuffer>(m_Device.get(), vertices);
        m_IndexBuffer = std::make_unique<IndexBuffer>(m_Device.get(), indices);
        m_Texture = std::make_unique<Texture>(m_Device.get(), "Assets/texture.jpg");
        CreateTextureSampler();
        CreateUniformBuffers();
        CreateDescriptorPool();
        CreateDescriptorSets();
        CreateCommandBuffers();
    }

    void RenderFrame()
    {
        uint32_t imageIndex;
        auto frameIndex = m_Swapchain->GetCurrentFrameIndex();
        auto result = m_Swapchain->AcquireNextImage(&imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            OnSwapchainRecreateRequired();
            return;
        }
        else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
            throw std::runtime_error("Failed to acquire swap chain image!");

        auto& commandBuffer = m_CommandBuffers[frameIndex];
        commandBuffer->Reset(0);
        RecordCommandBuffer(commandBuffer, imageIndex);

        UpdateUniformBuffer(frameIndex);

        m_Swapchain->ResetFence(); // Only reset the fence if we are submitting work
        m_Swapchain->SubmitCommandBuffer(m_CommandBuffers[frameIndex]->Get());

        result = m_Swapchain->Present(imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_Window->ResizeFlag)
        {
            OnSwapchainRecreateRequired();
            m_Window->ResizeFlag = false;
        }
        else if (result != VK_SUCCESS)
        {
            throw std::runtime_error("failed to present swap chain image!");
        }
    }

    void MainLoop()
    {
        while (!m_Window->ShouldClose())
        {
            glfwPollEvents();
            RenderFrame();
        }

        vkDeviceWaitIdle(m_Device->Get());
    }

    void Cleanup()
    {
        // Destroy command buffers
        for (auto& commandBuffer : m_CommandBuffers)
            commandBuffer.reset();

        // Destroy framebuffers
        for (auto& swapChainFramebuffer : m_Framebuffers)
            swapChainFramebuffer.reset();

        m_Swapchain.reset(); // Destroy swapchain

        vkDestroyImageView(m_Device->Get(), m_DepthImageView, nullptr);
        vkDestroyImage(m_Device->Get(), m_DepthImage, nullptr);
        vkFreeMemory(m_Device->Get(), m_DepthImageMemory, nullptr);

        for (size_t i = 0; i < Swapchain::FramesInFlight; i++)
        {
            vkDestroyBuffer(m_Device->Get(), m_UniformBuffers[i], nullptr);
            vkFreeMemory(m_Device->Get(), m_UniformBuffersMemory[i], nullptr);
        }

        vkDestroyDescriptorPool(m_Device->Get(), m_DescriptorPool, nullptr);
        vkDestroyDescriptorSetLayout(m_Device->Get(), m_DescriptorSetLayout, nullptr);

        m_IndexBuffer.reset(); // Destroy index buffer
        m_VertexBuffer.reset(); // Destroy vertex buffer
        m_Texture.reset(); // Destroy texture

        vkDestroySampler(m_Device->Get(), m_TextureSampler, nullptr);

        vkDestroyPipeline(m_Device->Get(), m_Pipeline, nullptr);
        vkDestroyPipelineLayout(m_Device->Get(), m_PipelineLayout, nullptr);

        vkDestroyRenderPass(m_Device->Get(), m_RenderPass, nullptr);

        m_Device.reset(); // Destroy device

        vkDestroySurfaceKHR(m_Instance->Get(), m_Surface, nullptr);

        m_Instance.reset(); // Destroy vulkan instance
        m_Window.reset(); // Destroy window
    }

    std::unique_ptr<Window> m_Window;
    std::unique_ptr<VulkanInstance> m_Instance;
    std::unique_ptr<VulkanDevice> m_Device;
    std::unique_ptr<Swapchain> m_Swapchain;
    std::unique_ptr<VertexBuffer> m_VertexBuffer;
    std::unique_ptr<IndexBuffer> m_IndexBuffer;
    std::unique_ptr<Texture> m_Texture;

    VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
    VkRenderPass m_RenderPass = VK_NULL_HANDLE;
    VkDescriptorSetLayout m_DescriptorSetLayout = VK_NULL_HANDLE;
    VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;
    VkPipeline m_Pipeline = VK_NULL_HANDLE;
    VkDescriptorPool m_DescriptorPool = VK_NULL_HANDLE;
    VkSampler m_TextureSampler = VK_NULL_HANDLE;

    VkImage m_DepthImage = VK_NULL_HANDLE;
    VkDeviceMemory m_DepthImageMemory = VK_NULL_HANDLE;
    VkImageView m_DepthImageView = VK_NULL_HANDLE;

    std::vector<std::unique_ptr<Framebuffer>> m_Framebuffers;
    std::vector<VkDescriptorSet> m_DescriptorSets;
    std::vector<VkBuffer> m_UniformBuffers;
    std::vector<VkDeviceMemory> m_UniformBuffersMemory;
    std::vector<void*> m_UniformBuffersMapData;
    std::vector<std::unique_ptr<CommandBuffer>> m_CommandBuffers;

    uint32_t m_FrameCounter = 0;
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
