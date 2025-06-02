#version 330

layout(std140) uniform;

layout(location = 0) in vec4 aPosition;
layout(location = 3) in vec2 aTexCoord;

uniform mat4 uLocalToViewMatrix;

uniform ProjectionBlock
{
	mat4 uViewToClipMatrix;
};

out VertexData
{
	vec2 texCoord;
} Out;

void main()
{
	gl_Position = uViewToClipMatrix * (uLocalToViewMatrix * aPosition);
	Out.texCoord = aTexCoord;
}
