#version 330

layout(location = 0) in vec4 aPosition;
layout(location = 2) in vec3 aNormal;

uniform mat4 uLocalToViewMatrix;
uniform mat3 uLocalToViewNormalMatrix;
uniform vec4 uDiffuseColor;

layout(std140) uniform ProjectionBlock
{
    mat4 uViewToClipMatrix;
};

smooth out vec4 iViewPosition;
smooth out vec4 iDiffuseColor;
smooth out vec3 iViewNormal;

void main()
{
    vec4 viewPosition = uLocalToViewMatrix * aPosition;
    gl_Position = uViewToClipMatrix * viewPosition;

    iViewPosition = viewPosition;
    iDiffuseColor = uDiffuseColor;
    iViewNormal = uLocalToViewNormalMatrix * aNormal;
}
