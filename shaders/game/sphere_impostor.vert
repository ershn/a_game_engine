#version 330

layout(location = 0) in vec3 aViewPosition;
layout(location = 1) in float aRadius;

out VertexData
{
    vec3 sphereViewPosition;
    float sphereRadius;
} Out;

void main()
{
    Out.sphereViewPosition = aViewPosition;
    Out.sphereRadius = aRadius;
}
