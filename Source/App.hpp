#pragma once

#include "glm.hpp"
#include "vulkan/vulkan.h"

#include "Pipeline.hpp"
#include "Window.hpp"
#include "SwapChain.hpp"
#include "Model.hpp"

#include <memory>
#include <cstdint>
#include <vector>

namespace lv
{
    class App
    {
    public:
        explicit App(const glm::uvec2 windowSize);
        ~App();

        App (const App&) = delete;
        App operator = (const App&) = delete;

        void Run();
    private:
        Window m_Window;
        Device m_Device;
        std::unique_ptr<SwapChain> m_SwapChain;

        VkPipelineLayout m_PipelineLayout;
        std::unique_ptr<Pipeline> m_Pipeline;
        std::vector<VkCommandBuffer> m_CommandBuffers;
        std::unique_ptr<Model> m_Model;

        void CreatePipelineLayout();
        void CreatePipeline();
        void CreateCommandBuffers();
        void DrawFrame();
        void LoadModels();
        void RecreateSwapChain();
        void RecordCommandBuffer(int imageIndex);
    };
}
