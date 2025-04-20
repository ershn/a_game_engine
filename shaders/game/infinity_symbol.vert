#version 330

layout(std140) uniform;

layout(location = 0) in vec4 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTextureCoord;

uniform mat4 uLocalToViewMatrix;
uniform mat3 uLocalToViewNormalMatrix;

uniform ProjectionBlock
{
    mat4 uViewToClipMatrix;
};

out VertexData
{
    vec4 viewPosition;
    vec3 viewNormal;
    vec2 textureCoord;
} Out;

void main()
{
    vec4 viewPosition = uLocalToViewMatrix * aPosition;
    gl_Position = uViewToClipMatrix * viewPosition;

    Out.viewPosition = viewPosition;
    Out.viewNormal = uLocalToViewNormalMatrix * aNormal;
    Out.textureCoord = aTextureCoord;
}
