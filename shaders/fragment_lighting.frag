#version 330

smooth in vec4 iCameraPosition;
smooth in vec4 iColor;
smooth in vec3 iCameraNormal;

uniform vec3 uCameraLightPosition;
uniform vec4 uLightIntensity;
uniform float uLightAttenuation;
uniform vec4 uAmbientLightIntensity;
uniform vec4 uSpecularColor;
uniform float uSurfaceShininess;

// unused
layout(std140) uniform FragmentPositionData
{
    mat4 uClipToCameraMatrix;
    ivec2 uViewportDimensions;
};

out vec4 oColor;

// unused
vec3 camera_position_from_fragment_coord()
{
    vec4 ndc_position;
    ndc_position.xy = 2.0 * gl_FragCoord.xy / uViewportDimensions - 1.0;
    ndc_position.z = (2.0 * gl_FragCoord.z - gl_DepthRange.near - gl_DepthRange.far) / (gl_DepthRange.far - gl_DepthRange.near);
    ndc_position.w = 1.0;

    vec4 clip_position = ndc_position / gl_FragCoord.w;

    return (uClipToCameraMatrix * clip_position).xyz;
}

vec4 attenuated_light_intensity(in vec3 cameraPosition, out vec3 directionToLight)
{
    vec3 differenceToLight = uCameraLightPosition - cameraPosition;
    float distanceToLightSquare = dot(differenceToLight, differenceToLight);
    directionToLight = differenceToLight * inversesqrt(distanceToLightSquare);
    return uLightIntensity / (1.0 + uLightAttenuation * sqrt(distanceToLightSquare));
}

void main()
{
    vec3 cameraPosition = vec3(iCameraPosition);
    vec3 cameraNormal = normalize(iCameraNormal);

    vec3 directionToLight;
    vec4 attenuatedLightIntensity = attenuated_light_intensity(cameraPosition, directionToLight);

    float incidenceAngleCos = dot(directionToLight, cameraNormal);
    incidenceAngleCos = clamp(incidenceAngleCos, 0.0, 1.0);

    vec3 viewDirection = normalize(-cameraPosition);
    vec3 lightReflectionDirection = reflect(-directionToLight, cameraNormal);
    float phongTerm = dot(viewDirection, lightReflectionDirection);
    phongTerm = clamp(phongTerm, 0.0, 1.0);
    phongTerm = incidenceAngleCos != 0.0 ? phongTerm : 0.0;
    phongTerm = pow(phongTerm, uSurfaceShininess);

    oColor = (iColor * 0.75) * attenuatedLightIntensity * incidenceAngleCos
      + uSpecularColor * attenuatedLightIntensity * phongTerm
      + iColor * uAmbientLightIntensity;
}
