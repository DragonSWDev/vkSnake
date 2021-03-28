#ifndef VULKANRENDERER_HPP
#define VULKANRENDERER_HPP

#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <vulkan/vulkan.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "external/vk_mem_alloc.h"

#include "VulkanPipeline.hpp"
#include "VertexInput.hpp"
#include "ReactangleShape.hpp"

//Main class of Vulkan renderer
//Initializes Vulkan and some needed things like command buffer, pipeline etc. and provide methods for rendering things
//Currently it only renders ReactangleShape object

struct ObjectPushConstants //Push constants 
{
    glm::vec4 colorVector;
    glm::mat4 mvpMatrix;
};

class VulkanRenderer
{
    public:
        bool initRenderer(SDL_Window* window, int width, int height, bool debug); //Init everything
        void destroyRenderer(); //Cleanup everything
        void render(); //Render everything

        void draw(ReactangleShape reactangleShape); //Add object to list

    private:
        bool initSuccessful;
        
        int frameNumber;

        VkExtent2D windowExtent;

        VkInstance vulkanInstance;
        VkPhysicalDevice physicalDevice;
        VkDevice vulkanDevice;
        VkDebugUtilsMessengerEXT debugMessenger;
        VkSurfaceKHR vulkanSurface;
        VkQueue graphicsQueue;
        uint32_t graphicsQueueFamily;
        VmaAllocator vmaAllocator;

        VkSwapchainKHR vulkanSwapchain;
        VkFormat swapchainImageFormat;
        std::vector<VkImage> swapchainImages;
        std::vector<VkImageView> swapchainImageViews;

        VkCommandPool commandPool;
        VkCommandBuffer mainCommandBuffer;

        VkRenderPass renderPass;
        std::vector<VkFramebuffer> framebuffers;

        VkSemaphore presentSemaphore, renderSemaphore;
        VkFence renderFence;

        VkShaderModule vertexShader, fragmentShader;
        VulkanPipeline pipeline;
        VkPipelineLayout pipelineLayout;

        VmaAllocation allocation;
        VkBuffer buffer;

        VertexInput reactangleShape;

        std::vector <ReactangleShape> drawableShapes; //List of objects to draw

        void initVulkan(SDL_Window* window, bool debug); //Instance, physical device selection and logical device creation
        void createSwapchain(); //Swapchain creation
        void createCommands(); //Command pool and command buffer creation
        void initDefaultRenderPass(); //Init default render pass
        void initFramebuffers(); //Framebuffers initialization
        void initSyncStructures(); //Fence and semaphores initialization
        void createReactangleShape(); //Rectangle shape setup (setup vertex input and allocates buffers)

        VkShaderModule createShaderModule(const char* fileName); //Loading and creating shader module
        void initPipeline(); //Initliazing pipeline
};

#endif
