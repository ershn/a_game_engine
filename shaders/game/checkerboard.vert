#version 330

layout(std140) uniform;

layout(location = 0) in vec4 inPosition;
layout(location = 3) in vec2 inTexCoord;

uniform mat4 _localToViewMatrix;

uniform ProjectionBlock
{
    mat4 _viewToClipMatrix;
};

out Varyings
{
    vec2 texCoord;
} Out;

void main()
{
    gl_Position = _viewToClipMatrix * (_localToViewMatrix * inPosition);
    Out.texCoord = inTexCoord;
}
