#ifndef VERTEXINPUT_HPP
#define VERTEXINPUT_HPP

#include <glm/vec3.hpp>
#include <vector>
#include <vulkan/vulkan.h>

//Vertex buffer
//Used to define and setup vertex buffer for Vulkan pipeline

struct VertexData
{
    glm::vec3 position;
    glm::vec3 color;
};

class VertexInput
{
private:
    std::vector<VkVertexInputBindingDescription> bindingDescriptions;
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions;

public:
    std::vector <VertexData> vertices;

    VertexInput();

    int getBindingDescriptionsCount();
    VkVertexInputBindingDescription* getBindingDescriptions();

    int getAttributeDescriptionsCount();
    VkVertexInputAttributeDescription* getAttributeDescriptions();

    size_t getShapeDataSize();
};

#endif
