#version 330

layout(location = 0) in vec4 aPosition;

uniform mat4 uLocalToViewMatrix;
uniform vec4 uColor;

layout(std140) uniform ProjectionBlock
{
    mat4 uViewToClipMatrix;
};

smooth out vec4 iColor;

void main()
{
    gl_Position = uViewToClipMatrix * uLocalToViewMatrix * aPosition;
    iColor = uColor;
}
