#version 330

in VertexData
{
	vec2 texCoord;
} In;

uniform sampler2D uTexture;

uniform GammaCorrectionBlock
{
	float gammaInverse;
};

out vec4 oColor;

void main()
{
	vec4 color = texture(uTexture, In.texCoord);
	oColor = pow(color, vec4(gammaInverse, gammaInverse, gammaInverse, 1.0));
}
