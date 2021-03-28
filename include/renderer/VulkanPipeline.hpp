#ifndef VULKANPIPELINE_HPP
#define VULKANPIPELINE_HPP

#include <vector>
#include <vulkan/vulkan.h>

//Vulkan pipeline builder
//It needs some parameters (like shader stages, vertex input info, pipeline layout etc.) then it builds Vulkan pipeline which can be used to render

class VulkanPipeline
{
private:
    VkViewport viewport;
    VkRect2D scissor;

    std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
    VkPipelineVertexInputStateCreateInfo vertexInputInfo;
    VkPipelineInputAssemblyStateCreateInfo inputAssembly;
    VkPipelineRasterizationStateCreateInfo rasterizer;
    VkPipelineMultisampleStateCreateInfo multisampling;
    VkPipelineColorBlendAttachmentState colorBlendAttachment;

    VkPipeline pipeline;
    VkPipelineLayout pipelineLayout;

public:
    VkPipeline getPipeline();

    bool createPipeline(VkDevice device, VkRenderPass renderPass);
    void destroyPipeline(VkDevice device);

    void setViewport(VkViewport viewport);
    void setScissor(VkRect2D scissor);
    void setPipelineLayout(VkPipelineLayout pipelineLayout);

    void addShaderStage(VkShaderStageFlagBits shaderStage, VkShaderModule shaderModule);
    void setVertexInputState(int bindingCount, VkVertexInputBindingDescription* bindings, int attributeCount, VkVertexInputAttributeDescription* attributes);
    void setInputAssembly(VkPrimitiveTopology topology);
    void setRasterizer(VkPolygonMode polygonMode);
    void setMultisampling();
    void setColorBlendAttachment();
};

#endif
