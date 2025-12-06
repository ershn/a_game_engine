#version 330

layout(std140) uniform;

layout(location = 0) in vec4 inPosition;

uniform mat4 uLocalToViewMatrix;
uniform vec3 uColor;

uniform ProjectionBlock
{
    mat4 _viewToClipMatrix;
};

smooth out vec3 varColor;

void main()
{
    gl_Position = _viewToClipMatrix * uLocalToViewMatrix * inPosition;
    varColor = uColor;
}
