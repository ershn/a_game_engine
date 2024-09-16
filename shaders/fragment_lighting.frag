#version 330

smooth in vec3 iModelPosition;
smooth in vec4 iModelColor;
smooth in vec3 iModelNormal;

uniform vec3 uModelLightPosition;
uniform vec4 uLightIntensity;
uniform vec4 uAmbientLightIntensity;

out vec4 oColor;

void main()
{
    vec3 lightDirection = normalize(uModelLightPosition - iModelPosition);
    float incidenceAngleCos = dot(lightDirection, normalize(iModelNormal));
    incidenceAngleCos = clamp(incidenceAngleCos, 0.0, 1.0);

    oColor = iModelColor * uLightIntensity * incidenceAngleCos + iModelColor * uAmbientLightIntensity;
}
