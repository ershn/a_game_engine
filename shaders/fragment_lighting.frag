#version 330

smooth in vec4 iCameraPosition;
smooth in vec4 iColor;
smooth in vec3 iCameraNormal;

uniform vec4 uSpecularColor;
uniform float uSurfaceShininess;

layout(std140) uniform LightDataBlock
{
    vec4 uLightIntensity;
    vec4 uAmbientLightIntensity;
    vec3 uCameraLightPosition;
    float uLightAttenuation;
};

// unused
layout(std140) uniform FragmentPositionDataBlock
{
    mat4 uClipToCameraMatrix;
    ivec2 uViewportDimensions;
};

out vec4 oColor;

// unused
vec3 cameraPositionFromFragmentCoord()
{
    vec4 ndcPosition;
    ndcPosition.xy = 2.0 * gl_FragCoord.xy / uViewportDimensions - 1.0;
    ndcPosition.z = (2.0 * gl_FragCoord.z - gl_DepthRange.near - gl_DepthRange.far) / (gl_DepthRange.far - gl_DepthRange.near);
    ndcPosition.w = 1.0;

    vec4 clipPosition = ndcPosition / gl_FragCoord.w;

    return (uClipToCameraMatrix * clipPosition).xyz;
}

vec4 attenuatedLightIntensity(in vec3 cameraPosition, out vec3 directionToLight)
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
    vec4 attenuatedLight = attenuatedLightIntensity(cameraPosition, directionToLight);

    float incidenceAngleCos = dot(directionToLight, cameraNormal);
    incidenceAngleCos = clamp(incidenceAngleCos, 0.0, 1.0);

    vec3 viewDirection = normalize(-cameraPosition);

    vec3 halfAngleDirection = normalize(viewDirection + directionToLight);
    float halfAngle = acos(dot(cameraNormal, halfAngleDirection));

    float gaussianExponent = halfAngle / uSurfaceShininess;
    gaussianExponent = -(gaussianExponent * gaussianExponent);
    float gaussianTerm = exp(gaussianExponent);
    gaussianTerm = incidenceAngleCos != 0.0 ? gaussianTerm : 0.0;

    oColor = iColor * attenuatedLight * 0.75 * incidenceAngleCos
      + uSpecularColor * attenuatedLight * 0.25 * gaussianTerm
      + iColor * uAmbientLightIntensity;
}
