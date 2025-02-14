#version 330

smooth out vec2 iMapping;

uniform vec3 uSphereViewPosition;
uniform float uSphereRadius;

layout(std140) uniform ProjectionBlock
{
    mat4 uViewToClipMatrix;
};

void main()
{
    vec2 positionOffset;

    switch (gl_VertexID)
    {
    case 0:
		iMapping = vec2(-1.0, 1.0);
        positionOffset = vec2(-uSphereRadius, uSphereRadius);
		break;
    case 1:
		iMapping = vec2(1.0, 1.0);
        positionOffset = vec2(uSphereRadius, uSphereRadius);
		break;
    case 2:
		iMapping = vec2(-1.0, -1.0);
        positionOffset = vec2(-uSphereRadius, -uSphereRadius);
		break;
    case 3:
		iMapping = vec2(1.0, -1.0);
        positionOffset = vec2(uSphereRadius, -uSphereRadius);
		break;
    }

    vec4 viewPosition = vec4(uSphereViewPosition, 1.0);
    viewPosition.xy += positionOffset;

    gl_Position = uViewToClipMatrix * viewPosition;
}
