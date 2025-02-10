#version 330

smooth in vec4 iColor;

layout(std140) uniform GammaCorrectionBlock
{
	float gammaInverse;
};

out vec4 oColor;

void main()
{
    oColor = pow(iColor, vec4(vec3(gammaInverse), 1.0));
}
