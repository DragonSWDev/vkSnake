#include "VulkanRenderer.hpp"

#include "external/VkBootstrap.h"

#define VMA_IMPLEMENTATION
#include "external/vk_mem_alloc.h"

#include <iostream>
#include <fstream>

bool VulkanRenderer::initRenderer(SDL_Window* window, int width, int height, bool debug)
{
    //If some step will fail then this variable will become false
    initSuccessful = true;

    frameNumber = 0;

    windowExtent.width = width;
    windowExtent.height = height;

    initVulkan(window, debug);
    createSwapchain();
    createCommands();
    initDefaultRenderPass();
    initFramebuffers();
    initSyncStructures();

    createReactangleShape();
    initPipeline();

    return initSuccessful;
}

//Cleanup everything
void VulkanRenderer::destroyRenderer()
{
    vkDeviceWaitIdle(vulkanDevice); //Make sure everything finished before cleaning

    vmaDestroyBuffer(vmaAllocator, buffer, allocation); //Destroy vertex buffer data

    vkDestroyPipelineLayout(vulkanDevice, pipelineLayout, nullptr); //Destroy pipeline layout

    pipeline.destroyPipeline(vulkanDevice); //Destroy pipeline

    vkDestroyShaderModule(vulkanDevice, vertexShader, nullptr); //Fragment shader
    vkDestroyShaderModule(vulkanDevice, fragmentShader, nullptr);

    vkDestroyFence(vulkanDevice, renderFence, nullptr); //Fence

    vkDestroySemaphore(vulkanDevice, presentSemaphore, nullptr); //Semaphores
    vkDestroySemaphore(vulkanDevice, renderSemaphore, nullptr);

    vkDestroyCommandPool(vulkanDevice, commandPool, nullptr); //Command pool (will also destroy command buffers)

    vkDestroyRenderPass(vulkanDevice, renderPass, nullptr); //Render pass

    for (int i = 0; i < swapchainImageViews.size(); i++) //Framebuffers ans swapchain image views
    {
        vkDestroyFramebuffer(vulkanDevice, framebuffers[i], nullptr);

        vkDestroyImageView(vulkanDevice, swapchainImageViews[i], nullptr);
    }

    vkDestroySwapchainKHR(vulkanDevice, vulkanSwapchain, nullptr); //Swapchain

    vkDestroySurfaceKHR(vulkanInstance, vulkanSurface, nullptr); //Surface

    vmaDestroyAllocator(vmaAllocator);

    vkDestroyDevice(vulkanDevice, nullptr); //Logical device

    vkb::destroy_debug_utils_messenger(vulkanInstance, debugMessenger, nullptr); //Debug messenger

    vkDestroyInstance(vulkanInstance, nullptr); //Instance
}

//Render things here
//It should check for errors as well
void VulkanRenderer::render()
{
    //Wait for render to finish
    vkWaitForFences(vulkanDevice, 1, &renderFence, true, 1000000000);
    vkResetFences(vulkanDevice, 1, &renderFence);

    //Get image from swap chain
    uint32_t swapchainImageIndex;
    vkAcquireNextImageKHR(vulkanDevice, vulkanSwapchain, 1000000000, presentSemaphore, nullptr, &swapchainImageIndex);

    //Reste command buffer
    vkResetCommandBuffer(mainCommandBuffer, 0);

    //Setup command buffer
    VkCommandBufferBeginInfo commandBufferBeginInfo = {};
    commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    commandBufferBeginInfo.pNext = nullptr;
    commandBufferBeginInfo.pInheritanceInfo = nullptr;
    commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(mainCommandBuffer, &commandBufferBeginInfo);

    //Clear screen
    VkClearValue clearValue;
    clearValue.color = { { 0.0f, 0.0f, 0.0f, 1.0f } };

    //Setup renderpass
    VkRenderPassBeginInfo renderPassBeginInfo = {};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.pNext = nullptr;
    renderPassBeginInfo.renderPass = renderPass;
    renderPassBeginInfo.renderArea.offset.x = 0;
    renderPassBeginInfo.renderArea.offset.y = 0;
    renderPassBeginInfo.renderArea.extent = windowExtent;
    renderPassBeginInfo.framebuffer = framebuffers[swapchainImageIndex];
    
    renderPassBeginInfo.clearValueCount = 1;
    renderPassBeginInfo.pClearValues = &clearValue;

    //Start rendering things
    vkCmdBeginRenderPass(mainCommandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    //Bind pipeline
    vkCmdBindPipeline(mainCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.getPipeline()); 

    //Bind vertex buffers
    VkDeviceSize offset = 0;
    vkCmdBindVertexBuffers(mainCommandBuffer, 0, 1, &buffer, &offset);
    
    //Setup MVP matrix
    glm::mat4 view = glm::mat4(1.0f);

    glm::mat4 projection = glm::ortho(0.0f, (float)windowExtent.width, 0.0f, (float)windowExtent.height, 0.1f, 100.0f);

    //Draw all shapes
    for (ReactangleShape shape : drawableShapes)
    {
        //Setup model matrix for every shape
        //Origin of every shape is selected to be in top left corner so translating should move shape to desired position
        //and add half of width and height to that position
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(shape.x + (shape.width / 2), shape.y + (shape.height / 2), -1.0f));
        model = glm::scale(model, glm::vec3(shape.width / 2, shape.height / 2, 0.0f));

        //Setup push constants
        ObjectPushConstants pushConstants;
        pushConstants.colorVector = glm::vec4(shape.r, shape.g, shape.b, 1.0f);
        pushConstants.mvpMatrix = projection * view * model;

        //Send push constants data to GPU
        vkCmdPushConstants(mainCommandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(ObjectPushConstants), &pushConstants);

        //Draw things
        vkCmdDraw(mainCommandBuffer, reactangleShape.vertices.size(), 1, 0, 0);
    }

    //End of rendering
    vkCmdEndRenderPass(mainCommandBuffer);

    vkEndCommandBuffer(mainCommandBuffer);

    //Submit info
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext = nullptr;

    VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    submitInfo.pWaitDstStageMask = &waitStage;

    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &presentSemaphore;

    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &renderSemaphore;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &mainCommandBuffer;

    //Add to queue
    vkQueueSubmit(graphicsQueue, 1, &submitInfo, renderFence);

    //Setup presentation and present things
    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.pNext = nullptr;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &vulkanSwapchain;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &renderSemaphore;
    presentInfo.pImageIndices = &swapchainImageIndex;

    vkQueuePresentKHR(graphicsQueue, &presentInfo);

    //Clear list of objects to render because every object were rendered
    drawableShapes.clear();

    //Go to next frame
    frameNumber++;
}

//Add object to list of objects to render
void VulkanRenderer::draw(ReactangleShape reactangleShape)
{ 
    drawableShapes.push_back(reactangleShape);
}

void VulkanRenderer::initVulkan(SDL_Window* window, bool debug)
{
    vkb::InstanceBuilder instanceBuilder;

    //Should probablably check for errors as well

    //Init instance
    auto builderInstance = instanceBuilder.set_app_name("vkSnake")
            .request_validation_layers(debug)
            .require_api_version(1, 0, 0)
            .use_default_debug_messenger()
            .build();

    vkb::Instance vkbInstance = builderInstance.value();

    vulkanInstance = vkbInstance.instance;
    debugMessenger = vkbInstance.debug_messenger;

    //Create SDL surface
    if (SDL_Vulkan_CreateSurface(window, vulkanInstance, &vulkanSurface) == SDL_FALSE)
    {
        initSuccessful = false;
        return;
    }

    //Pick physical device and setup logical device
    vkb::PhysicalDeviceSelector selector { vkbInstance };
    vkb::PhysicalDevice vkbPhysicalDevice = selector.set_minimum_version(1, 0)
                            .set_surface(vulkanSurface)
                            .select()
                            .value();

    vkb::DeviceBuilder deviceBuilder { vkbPhysicalDevice };

    vkb::Device vkbDevice = deviceBuilder.build().value();

    physicalDevice = vkbPhysicalDevice.physical_device;
    vulkanDevice = vkbDevice.device;

    //Get queue
    graphicsQueue = vkbDevice.get_queue(vkb::QueueType::graphics).value();
    graphicsQueueFamily = vkbDevice.get_queue_index(vkb::QueueType::graphics).value();

    //Setup allocator
    VmaAllocatorCreateInfo allocatorInfo = {};
    allocatorInfo.physicalDevice = physicalDevice;
    allocatorInfo.device = vulkanDevice;
    allocatorInfo.instance = vulkanInstance;

    vmaCreateAllocator(&allocatorInfo, &vmaAllocator);
}

//Setup swapchain
void VulkanRenderer::createSwapchain()
{
    vkb::SwapchainBuilder swapchainBuilder { physicalDevice, vulkanDevice, vulkanSurface };

    vkb::Swapchain vkbSwapchain = swapchainBuilder.use_default_format_selection()
                    .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
                    .set_desired_extent(windowExtent.width, windowExtent.height)
                    .build()
                    .value();

    vulkanSwapchain = vkbSwapchain.swapchain;
    swapchainImages = vkbSwapchain.get_images().value();
    swapchainImageViews = vkbSwapchain.get_image_views().value();

    swapchainImageFormat = vkbSwapchain.image_format;
}

//Create commands buffers
void VulkanRenderer::createCommands()
{
    VkCommandPoolCreateInfo commandPoolInfo = {};
    commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolInfo.pNext = nullptr;

    commandPoolInfo.queueFamilyIndex = graphicsQueueFamily;
    commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    if (vkCreateCommandPool(vulkanDevice, &commandPoolInfo, nullptr, &commandPool) != VK_SUCCESS)
    {
        initSuccessful = false;
        return;
    }

    VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
    commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.pNext = nullptr;
    commandBufferAllocateInfo.commandPool = commandPool;
    commandBufferAllocateInfo.commandBufferCount = 1;
    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

    if (vkAllocateCommandBuffers(vulkanDevice, &commandBufferAllocateInfo, &mainCommandBuffer))
    {
        initSuccessful = false;
        return;
    }
}

//Setup render pass
void VulkanRenderer::initDefaultRenderPass()
{
    VkAttachmentDescription colorAttachment = {};
    colorAttachment.format = swapchainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentReference = {};
    colorAttachmentReference.attachment = 0;
    colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpassDescription = {};
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.colorAttachmentCount = 1;
    subpassDescription.pColorAttachments = &colorAttachmentReference;

    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpassDescription;

    if (vkCreateRenderPass(vulkanDevice, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
    {
        initSuccessful = false;
        return;
    }
}

//Setup framebuffers
void VulkanRenderer::initFramebuffers()
{
    VkFramebufferCreateInfo framebufferInfo = {};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.pNext = nullptr;
    framebufferInfo.renderPass = renderPass;
    framebufferInfo.attachmentCount = 1;
    framebufferInfo.width = windowExtent.width;
    framebufferInfo.height = windowExtent.height;
    framebufferInfo.layers = 1;

    const uint32_t swapchainImageCount = swapchainImages.size();
    framebuffers = std::vector<VkFramebuffer>(swapchainImageCount);

    for (int i = 0; i < swapchainImageCount; i++)
    {
        framebufferInfo.pAttachments = &swapchainImageViews[i];

        if (vkCreateFramebuffer(vulkanDevice, &framebufferInfo, nullptr, &framebuffers[i]) != VK_SUCCESS)
        {
            initSuccessful = false;
            return;
        }
    }
}

//Setup fences and semaphores
void VulkanRenderer::initSyncStructures()
{
    VkFenceCreateInfo fenceCreateInfo = {};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.pNext = nullptr;
    fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    if (vkCreateFence(vulkanDevice, &fenceCreateInfo, nullptr, &renderFence) != VK_SUCCESS)
    {
        initSuccessful = false;
        return;
    }

    VkSemaphoreCreateInfo semaphoreCreateInfo = {};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphoreCreateInfo.pNext = nullptr;
    semaphoreCreateInfo.flags = 0;

    if (vkCreateSemaphore(vulkanDevice, &semaphoreCreateInfo, nullptr, &presentSemaphore) != VK_SUCCESS)
    {
        initSuccessful = false;
        return;
    }

    if (vkCreateSemaphore(vulkanDevice, &semaphoreCreateInfo, nullptr, &renderSemaphore) != VK_SUCCESS)
    {
        initSuccessful = false;
        return;
    }
}

//Setup vertex input
void VulkanRenderer::createReactangleShape()
{
    reactangleShape.vertices.resize(6);

    reactangleShape.vertices[0].position = { -1.0f, 1.0f, 0.0f };
    reactangleShape.vertices[1].position = { 1.0f, 1.0f, 0.0f };
    reactangleShape.vertices[2].position = { 1.0f, -1.0f, 0.0f };

    reactangleShape.vertices[3].position = { 1.0f, -1.0f, 0.0f };
    reactangleShape.vertices[4].position = { -1.0f, -1.0f, 0.0f };
    reactangleShape.vertices[5].position = { -1.0f, 1.0f, 0.0f };

    reactangleShape.vertices[0].color = { 1.0f, 0.0f, 0.0f };
    reactangleShape.vertices[1].color = { 1.0f, 0.0f, 0.0f };
    reactangleShape.vertices[2].color = { 1.0f, 0.0f, 0.0f };

    reactangleShape.vertices[3].color = { 1.0f, 0.0f, 0.0f };
    reactangleShape.vertices[4].color = { 1.0f, 0.0f, 0.0f };
    reactangleShape.vertices[5].color = { 1.0f, 0.0f, 0.0f };

    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = reactangleShape.getShapeDataSize();
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

    VmaAllocationCreateInfo vmaAllocationInfo = {};
    vmaAllocationInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

    if (vmaCreateBuffer(vmaAllocator, &bufferInfo, &vmaAllocationInfo, &buffer, &allocation, nullptr) != VK_SUCCESS)
    {
        initSuccessful = false;
        return;
    }

    void* data;
    vmaMapMemory(vmaAllocator, allocation, &data);
    memcpy(data, reactangleShape.vertices.data(), reactangleShape.getShapeDataSize());
    vmaUnmapMemory(vmaAllocator, allocation);
}

//Create shader module from selected file
VkShaderModule VulkanRenderer::createShaderModule(const char* fileName)
{
    std::ifstream spvShaderFile(fileName, std::ios::ate | std::ios::binary);

    if (!spvShaderFile.is_open())
    {
        return NULL;
    }

    size_t fileSize = (size_t)spvShaderFile.tellg();

    std::vector<char> fileBuffer(fileSize);

    spvShaderFile.seekg(0);
    spvShaderFile.read(fileBuffer.data(), fileSize);

    spvShaderFile.close();

    VkShaderModuleCreateInfo shaderCreateInfo = {};
    shaderCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderCreateInfo.pNext = nullptr;
    
    shaderCreateInfo.codeSize = fileBuffer.size();
    shaderCreateInfo.pCode = reinterpret_cast<const uint32_t*>(fileBuffer.data());

    VkShaderModule shaderModule;

    if(vkCreateShaderModule(vulkanDevice, &shaderCreateInfo, nullptr, &shaderModule) != VK_SUCCESS)
    {
        return NULL;
    }

    return shaderModule;
}

//Setup graphics pipeline
void VulkanRenderer::initPipeline()
{
    //Set push contants data
    VkPushConstantRange pushConstants;
    pushConstants.offset = 0;
    pushConstants.size = sizeof(ObjectPushConstants);
    pushConstants.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    //Create pipeline layout
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.pNext = nullptr;
    pipelineLayoutInfo.flags = 0;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pSetLayouts = nullptr;
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstants;

    
    if (vkCreatePipelineLayout(vulkanDevice, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
    {
        initSuccessful = false;
        return;
    }

    vertexShader = createShaderModule("vertexshader.spv");

    if (vertexShader == NULL)
    {
        initSuccessful = false;
        return;
    }

    fragmentShader = createShaderModule("fragmentshader.spv");

    if (fragmentShader == NULL)
    {
        initSuccessful = false;
        return;
    }

    pipeline.addShaderStage(VK_SHADER_STAGE_VERTEX_BIT, vertexShader);
    pipeline.addShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, fragmentShader);

    //Bind vertex buffer
    pipeline.setVertexInputState(reactangleShape.getBindingDescriptionsCount(), reactangleShape.getBindingDescriptions(),
        reactangleShape.getAttributeDescriptionsCount(), reactangleShape.getAttributeDescriptions());

    pipeline.setInputAssembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

    VkViewport viewport;
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)windowExtent.width;
    viewport.height = (float)windowExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    pipeline.setViewport(viewport);

    VkRect2D scissor;
    scissor.offset = { 0, 0 };
    scissor.extent = windowExtent;

    pipeline.setScissor(scissor);

    pipeline.setRasterizer(VK_POLYGON_MODE_FILL);
    pipeline.setMultisampling();
    pipeline.setColorBlendAttachment();
    pipeline.setPipelineLayout(pipelineLayout);
    
    if (!pipeline.createPipeline(vulkanDevice, renderPass))
    {
        initSuccessful = false;

        return;
    }
}
