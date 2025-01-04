#include "App.hpp"

#include <stdexcept>
#include <array>

namespace lv
{

    App::App(const glm::uvec2 windowSize)
        : m_Window(lv::Window(windowSize, "LearVulkan")), m_Device(m_Window),
          m_SwapChain(m_Device, m_Window.GetExtent())
    {
        LoadModels();
        CreatePipelineLayout();
        CreatePipeline();
        CreateCommandBuffers();
    }

    App::~App()
    {
        vkDestroyPipelineLayout(m_Device.device(), m_PipelineLayout, nullptr);
    }

    void App::Run()
    {
        while (!m_Window.ShouldClose())
        {
            glfwPollEvents();
            DrawFrame();
        }

        vkDeviceWaitIdle(m_Device.device());
    }

    void App::CreatePipelineLayout()
    {
        VkPipelineLayoutCreateInfo pipelineLayoutInfo { };
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;

        if (vkCreatePipelineLayout(m_Device.device(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create pipeline layout!");
        }
    }

    void App::CreatePipeline()
    {
        auto pipelineConfig = lv::Pipeline::DefaultPipelineConfigInfo(m_SwapChain.width(), m_SwapChain.height());
        pipelineConfig.renderPass = m_SwapChain.getRenderPass();
        pipelineConfig.pipelineLayout = m_PipelineLayout;

        m_Pipeline = std::make_unique<lv::Pipeline>(m_Device,pipelineConfig,
        "Shaders/simple_shader.vert.spv",
        "Shaders/simple_shader.frag.spv");
    }

    void App::CreateCommandBuffers()
    {
        m_CommandBuffers.resize(m_SwapChain.imageCount());

        VkCommandBufferAllocateInfo allocateInfo { };

        allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocateInfo.commandPool = m_Device.getCommandPool();
        allocateInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size());

        if (vkAllocateCommandBuffers(m_Device.device(), &allocateInfo, m_CommandBuffers.data()) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate command buffers!");
        }

        for (size_t i = 0; i < m_CommandBuffers.size(); i++)
        {
            VkCommandBufferBeginInfo beginInfo { };
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

            if (vkBeginCommandBuffer(m_CommandBuffers[i], &beginInfo) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to begin recording command buffer!");
            }

            VkRenderPassBeginInfo renderPassInfo { };
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = m_SwapChain.getRenderPass();
            renderPassInfo.framebuffer = m_SwapChain.getFrameBuffer(i);

            renderPassInfo.renderArea.offset = {0, 0};
            renderPassInfo.renderArea.extent = m_SwapChain.getSwapChainExtent();

            std::array<VkClearValue, 2> clearValues { };
            clearValues[0].color = { 0.1, 0.1, 0.1, 1.0};
            clearValues[1].depthStencil = {1.0, 0};
            renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
            renderPassInfo.pClearValues = clearValues.data();

            vkCmdBeginRenderPass(m_CommandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

            m_Pipeline->Bind(m_CommandBuffers[i]);

            m_Model->Bind(m_CommandBuffers[i]);
            m_Model->Draw(m_CommandBuffers[i]);

            vkCmdEndRenderPass(m_CommandBuffers[i]);
            if (vkEndCommandBuffer(m_CommandBuffers[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to record command buffer!");
            }
        }
    }

    void App::DrawFrame()
    {
        uint32_t imageIndex;

        auto result = m_SwapChain.acquireNextImage(&imageIndex);

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            throw std::runtime_error("failed to acquire swap chain index!");
        }

        result = m_SwapChain.submitCommandBuffers(&m_CommandBuffers[imageIndex], &imageIndex);

        if (result != VK_SUCCESS)
        {
            throw std::runtime_error("failed to present swap chain image!");
        }
    }

    void App::LoadModels()
    {
        std::vector<Model::Vertex> vertices
        {
            {{0.0f, -0.5f}},
            {{0.5f, 0.5f}},
            {{-0.5f, 0.5f}}
        };

        m_Model = std::make_unique<Model>(m_Device, vertices);
    }
}
