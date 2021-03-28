#include "VertexInput.hpp"

//Setup binding and attributes
VertexInput::VertexInput()
{
    VkVertexInputBindingDescription mainBinding = {};
    mainBinding.binding = 0;
    mainBinding.stride = sizeof(VertexData);
    mainBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    bindingDescriptions.push_back(mainBinding);

    VkVertexInputAttributeDescription positionAttribute = {};
    positionAttribute.binding = 0;
    positionAttribute.location = 0;
    positionAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
    positionAttribute.offset = offsetof(struct VertexData, position);

    attributeDescriptions.push_back(positionAttribute);

    VkVertexInputAttributeDescription colorAttribute = {};
    colorAttribute.binding = 0;
    colorAttribute.location = 1;
    colorAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
    colorAttribute.offset = offsetof(struct VertexData, color);

    attributeDescriptions.push_back(colorAttribute);
}

//Return binding and attributes
int VertexInput::getBindingDescriptionsCount()
{
    return bindingDescriptions.size();
}

VkVertexInputBindingDescription* VertexInput::getBindingDescriptions()
{
    return bindingDescriptions.data();
}

int VertexInput::getAttributeDescriptionsCount()
{
    return attributeDescriptions.size();
}

VkVertexInputAttributeDescription* VertexInput::getAttributeDescriptions()
{
    return attributeDescriptions.data();
}

size_t VertexInput::getShapeDataSize()
{
    return vertices.size() * sizeof(VertexData);
}
