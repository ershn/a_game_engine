#version 330

layout(std140) uniform;

layout(location = 0) in vec4 inPosition;

uniform mat4 uLocalToViewMatrix;
uniform vec4 uColor;

uniform ProjectionBlock
{
    mat4 _viewToClipMatrix;
};

smooth out vec4 interpColor;

void main()
{
    gl_Position = _viewToClipMatrix * uLocalToViewMatrix * inPosition;
    interpColor = uColor;
}
