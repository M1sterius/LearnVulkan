#include "GraphicsPipeline.hpp"

GraphicsPipeline::GraphicsPipeline(const std::filesystem::path& vertPath, const std::filesystem::path& fragPath,
                                   const GraphicsPipelineConfig& config)
{

}

GraphicsPipeline::~GraphicsPipeline()
{

}

GraphicsPipelineConfig GraphicsPipeline::GetDefaultConfig()
{
    return GraphicsPipelineConfig();
}
