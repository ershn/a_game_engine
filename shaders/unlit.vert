#version 330

layout(std140) uniform;

layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec3 inDiffuseColor;

uniform mat4 uLocalToViewMatrix;

uniform ProjectionBlock
{
    mat4 _viewToClipMatrix;
};

smooth out vec3 interpColor;

void main()
{
    gl_Position = _viewToClipMatrix * (uLocalToViewMatrix * inPosition);
    interpColor = inDiffuseColor;
}
