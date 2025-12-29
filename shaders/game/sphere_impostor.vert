#version 330

layout(location = 0) in vec3 inViewPosition;
layout(location = 1) in float inRadius;

out Varyings
{
    vec3 sphereViewPosition;
    float sphereRadius;
} Out;

void main()
{
    Out.sphereViewPosition = inViewPosition;
    Out.sphereRadius = inRadius;
}
