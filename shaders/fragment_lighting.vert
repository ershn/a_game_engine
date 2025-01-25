#version 330

layout(location = 0) in vec4 aPosition;
layout(location = 1) in vec4 aColor;
layout(location = 2) in vec3 aNormal;

uniform mat4 uModelToCameraMatrix;
uniform mat3 uModelToCameraNormalMatrix;

layout(std140) uniform ProjectionBlock
{
    mat4 uCameraToClipMatrix;
};

smooth out vec4 iCameraPosition;
smooth out vec4 iColor;
smooth out vec3 iCameraNormal;

void main()
{
    vec4 cameraPosition = uModelToCameraMatrix * aPosition;
    gl_Position = uCameraToClipMatrix * cameraPosition;

    iCameraPosition = cameraPosition;
    iColor = aColor;
    iCameraNormal = uModelToCameraNormalMatrix * aNormal;
}
