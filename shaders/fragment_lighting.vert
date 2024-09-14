#version 330

layout(location = 0) in vec4 aPosition;
layout(location = 1) in vec3 aNormal;

uniform mat4 uModelToCameraMatrix;

layout(std140) uniform SharedMatrices
{
    mat4 uCameraToClipMatrix;
};

smooth out vec3 iModelPosition;
smooth out vec3 iModelNormal;

void main()
{
    gl_Position = uCameraToClipMatrix * (uModelToCameraMatrix * aPosition);

    iModelPosition = vec3(aPosition);
    iModelNormal = aNormal;
}
