#version 330

const float PI = 3.14159f;
const float TAU = 2.0f * PI;

layout(location = 0) in vec4 aPosition;
layout(location = 1) in vec3 aNormal;

uniform mat4 uModelToCameraMatrix;
uniform mat3 uNormalModelToCameraMatrix;

layout(std140) uniform SharedMatrices
{
    mat4 uCameraToClipMatrix;
};

uniform vec3 uDirectionToLight;
uniform vec4 uLightIntensity;

smooth out vec4 color;

void main()
{
    gl_Position = uCameraToClipMatrix * (uModelToCameraMatrix * aPosition);
    vec3 normal = normalize(uNormalModelToCameraMatrix * aNormal);

    float incidenceAngleCos = dot(normal, uDirectionToLight);
    incidenceAngleCos = clamp(incidenceAngleCos, 0.0f, 1.0f);
    color = vec4(1.0f, 0.2f, 0.2f, 1.0f) * uLightIntensity * incidenceAngleCos;
}