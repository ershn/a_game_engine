#version 330

in Varyings
{
	vec2 texCoord;
} In;

uniform sampler2D _texture;

out vec4 outColor;

void main()
{
	outColor = texture(_texture, In.texCoord);
}
