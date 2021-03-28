#include "VulkanPipeline.hpp"

//Setup pipeline creation info and create it
bool VulkanPipeline::createPipeline(VkDevice device, VkRenderPass renderPass)
{
    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.pNext = nullptr;

    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;

    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    VkPipelineColorBlendStateCreateInfo colorBlending = {};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.pNext = nullptr;

    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;

    VkGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.pNext = nullptr;

    pipelineInfo.stageCount = shaderStages.size();
    pipelineInfo.pStages = shaderStages.data();
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.renderPass = renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS)
    {
        return false;
    }

    return true;
}

void VulkanPipeline::destroyPipeline(VkDevice device)
{
    vkDestroyPipeline(device, pipeline, nullptr);
}

void VulkanPipeline::setViewport(VkViewport viewport)
{
    this->viewport = viewport;
}

void VulkanPipeline::setScissor(VkRect2D scissor)
{
    this->scissor = scissor;
}

void VulkanPipeline::setPipelineLayout(VkPipelineLayout pipelineLayout)
{
    this->pipelineLayout = pipelineLayout;
}

VkPipeline VulkanPipeline::getPipeline()
{
    return pipeline;
}

void VulkanPipeline::addShaderStage(VkShaderStageFlagBits shaderStage, VkShaderModule shaderModule)
{
    VkPipelineShaderStageCreateInfo shaderStageInfo = {};
    shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageInfo.pNext = nullptr;

    shaderStageInfo.stage = shaderStage;
    shaderStageInfo.module = shaderModule;

    shaderStageInfo.pName = "main";

    shaderStages.push_back(shaderStageInfo);
}

void VulkanPipeline::setVertexInputState(int bindingCount, VkVertexInputBindingDescription* bindings, int attributeCount, VkVertexInputAttributeDescription* attributes)
{
    vertexInputInfo = {};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.pNext = nullptr;

    vertexInputInfo.vertexBindingDescriptionCount = bindingCount;
    vertexInputInfo.pVertexBindingDescriptions = bindings;

    vertexInputInfo.vertexAttributeDescriptionCount = attributeCount;
    vertexInputInfo.pVertexAttributeDescriptions = attributes;
}

void VulkanPipeline::setInputAssembly(VkPrimitiveTopology topology)
{
    inputAssembly = {};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.pNext = nullptr;

    inputAssembly.topology = topology;

    inputAssembly.primitiveRestartEnable = VK_FALSE;
}

void VulkanPipeline::setRasterizer(VkPolygonMode polygonMode)
{
    rasterizer = {};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.pNext = nullptr;

    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;

    rasterizer.polygonMode = polygonMode;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_NONE;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f;
    rasterizer.depthBiasClamp = 0.0f;
    rasterizer.depthBiasSlopeFactor = 0.0f;
}

void VulkanPipeline::setMultisampling()
{
    multisampling = {};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.pNext = nullptr;

    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f;
    multisampling.pSampleMask = nullptr;
    multisampling.alphaToCoverageEnable = VK_FALSE;
    multisampling.alphaToOneEnable = VK_FALSE;
}

void VulkanPipeline::setColorBlendAttachment()
{
    colorBlendAttachment = {};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

    colorBlendAttachment.blendEnable = VK_FALSE;
}
