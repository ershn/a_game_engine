#version 330

layout(location = 0) in vec4 aPosition;
layout(location = 1) in vec4 aColor;

uniform mat4 uModelToCameraMatrix;

layout(std140) uniform SharedMatrices
{
    mat4 uCameraToClipMatrix;
};

smooth out vec4 iColor;

void main()
{
    gl_Position = uCameraToClipMatrix * uModelToCameraMatrix * aPosition;
    iColor = aColor;
}
