#version 330

in VertexData
{
	vec2 texCoord;
} In;

uniform sampler2D uTexture;

out vec4 oColor;

void main()
{
	oColor = texture(uTexture, In.texCoord);
}
