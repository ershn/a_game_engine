#version 330

in VertexData
{
    vec2 texCoord;
} In;

out vec4 oColor;

uniform sampler2D uTexture;

void main()
{
    oColor = texture(uTexture, In.texCoord);
}
