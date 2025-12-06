#version 330

layout(std140) uniform;

layout(location = 0) in vec4 inPosition;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inTexCoord;

uniform mat4 uLocalToViewMatrix;
uniform mat3 uLocalToViewNormalMatrix;

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
    vec4 viewPosition = uLocalToViewMatrix * inPosition;
    gl_Position = _viewToClipMatrix * viewPosition;

    Out.viewPosition = vec3(viewPosition);
    Out.viewNormal = uLocalToViewNormalMatrix * inNormal;
    Out.texCoord = inTexCoord;
}
