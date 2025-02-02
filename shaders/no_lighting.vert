#version 330

layout(location = 0) in vec4 aPosition;
layout(location = 1) in vec4 aColor;

uniform mat4 uLocalToViewMatrix;

layout(std140) uniform ProjectionBlock
{
    mat4 uViewToClipMatrix;
};

smooth out vec4 iColor;

void main()
{
    gl_Position = uViewToClipMatrix * uLocalToViewMatrix * aPosition;
    iColor = aColor;
}
