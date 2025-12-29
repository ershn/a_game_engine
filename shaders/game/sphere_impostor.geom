#version 330
#extension GL_EXT_gpu_shader4 : enable

layout(std140) uniform;

layout(points) in;
layout(triangle_strip, max_vertices=4) out;

in Varyings
{
    vec3 sphereViewPosition;
    float sphereRadius;
} In[];

out FragmentData
{
    flat vec3 sphereViewPosition;
    flat float sphereRadius;
    vec2 mapping;
};

uniform ProjectionBlock
{
    mat4 _viewToClipMatrix;
};

void main()
{
    const float squareCorrection = 1.5;

    sphereViewPosition = In[0].sphereViewPosition;
    sphereRadius = In[0].sphereRadius;
    mapping = vec2(-1.0, 1.0) * squareCorrection;
    gl_Position = _viewToClipMatrix * vec4(In[0].sphereViewPosition + vec3(-In[0].sphereRadius, In[0].sphereRadius, 0.0) * squareCorrection, 1.0);
    gl_PrimitiveID = gl_PrimitiveIDIn;
    EmitVertex();

    sphereViewPosition = In[0].sphereViewPosition;
    sphereRadius = In[0].sphereRadius;
    mapping = vec2(1.0, 1.0) * squareCorrection;
    gl_Position = _viewToClipMatrix * vec4(In[0].sphereViewPosition + vec3(In[0].sphereRadius, In[0].sphereRadius, 0.0) * squareCorrection, 1.0);
    gl_PrimitiveID = gl_PrimitiveIDIn;
    EmitVertex();

    sphereViewPosition = In[0].sphereViewPosition;
    sphereRadius = In[0].sphereRadius;
    mapping = vec2(-1.0, -1.0) * squareCorrection;
    gl_Position = _viewToClipMatrix * vec4(In[0].sphereViewPosition + vec3(-In[0].sphereRadius, -In[0].sphereRadius, 0.0) * squareCorrection, 1.0);
    gl_PrimitiveID = gl_PrimitiveIDIn;
    EmitVertex();

    sphereViewPosition = In[0].sphereViewPosition;
    sphereRadius = In[0].sphereRadius;
    mapping = vec2(1.0, -1.0) * squareCorrection;
    gl_Position = _viewToClipMatrix * vec4(In[0].sphereViewPosition + vec3(In[0].sphereRadius, -In[0].sphereRadius, 0.0) * squareCorrection, 1.0);
    gl_PrimitiveID = gl_PrimitiveIDIn;
    EmitVertex();
}
