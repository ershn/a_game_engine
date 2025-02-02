#version 330

const float PI = 3.14159f;
const float TAU = 2.0f * PI;

layout(location = 0) in vec4 aPosition;
layout(location = 1) in vec3 aNormal;

uniform mat4 uLocalToViewMatrix;
uniform mat3 uNormalLocalToViewMatrix;

layout(std140) uniform ProjectionBlock
{
    mat4 uViewToClipMatrix;
};

uniform vec3 uLightPosition;
uniform vec4 uLightIntensity;
uniform vec4 uAmbientLightIntensity;

smooth out vec4 color;

void main()
{
    vec4 diffuseColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);

    vec4 viewPosition = uLocalToViewMatrix * aPosition;
    gl_Position = uViewToClipMatrix * viewPosition;
    vec3 normal = normalize(uNormalLocalToViewMatrix * aNormal);

    vec3 lightDirection = normalize(uLightPosition - vec3(viewPosition));
    float incidenceAngleCos = dot(normal, lightDirection);
    incidenceAngleCos = clamp(incidenceAngleCos, 0.0f, 1.0f);
    color =
        diffuseColor * uLightIntensity * incidenceAngleCos + diffuseColor * uAmbientLightIntensity;
}
