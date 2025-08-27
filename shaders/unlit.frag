#version 330

smooth in vec3 interpColor;

out vec3 outColor;

void main()
{
    outColor = interpColor;
}
