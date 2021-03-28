#version 450

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aColor;

layout (location = 0) out vec4 outColor;

layout (push_constant) uniform constants
{
    vec4 colorVector;
    mat4 mvpMatrix;
} PushConstants;

void main()
{
    gl_Position = PushConstants.mvpMatrix * vec4(aPosition, 1.0f);
    outColor = PushConstants.colorVector;
}
