#version 330

layout(location = 0) in vec4 aPosition;

uniform mat4 uModelToCameraMatrix;
uniform vec4 uColor;

layout(std140) uniform SharedMatrices
{
    mat4 uCameraToClipMatrix;
};

smooth out vec4 iColor;

void main()
{
    gl_Position = uCameraToClipMatrix * uModelToCameraMatrix * aPosition;
    iColor = uColor;
}
