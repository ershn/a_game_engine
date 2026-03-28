#version 330

layout(std140) uniform;

layout(location = 0) in vec4 inPosition;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inTexCoord;

uniform mat4 _localToViewMatrix;
uniform mat3 _localToViewNormalMatrix;

uniform ProjectionBlock
{
    mat4 _viewToClipMatrix;
};

out Varyings
{
    vec3 viewPosition;
    vec3 viewNormal;
    vec2 texCoord;
} Out;

void main()
{
    vec4 viewPosition = _localToViewMatrix * inPosition;
    gl_Position = _viewToClipMatrix * viewPosition;

    Out.viewPosition = vec3(viewPosition);
    Out.viewNormal = _localToViewNormalMatrix * inNormal;
    Out.texCoord = inTexCoord;
}
