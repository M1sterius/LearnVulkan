#pragma once

#include "vulkan/vulkan.h"

#include <filesystem>

struct GraphicsPipelineConfig
{

};

class GraphicsPipeline
{
public:
    GraphicsPipeline(const std::filesystem::path& vertPath, const std::filesystem::path& fragPath, const GraphicsPipelineConfig& config);
    ~GraphicsPipeline();

    GraphicsPipeline(const GraphicsPipeline&) = delete;
    GraphicsPipeline operator = (const GraphicsPipeline&) = delete;

    static GraphicsPipelineConfig GetDefaultConfig();
};
