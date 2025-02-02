#version 330

smooth in vec4 iViewPosition;
smooth in vec4 iColor;
smooth in vec3 iViewNormal;

layout(std140) uniform;

uniform vec4 uSpecularColor;
uniform float uSurfaceShininess;

uniform MaterialBlock
{
    vec4 specularColor;
    float surfaceShininess;
} Material;

uniform LightDataBlock
{
    vec4 intensity;
    vec4 ambientIntensity;
    vec3 viewPosition;
    float attenuation;
} Light;

// unused
uniform FragmentPositionDataBlock
{
    mat4 uClipToViewMatrix;
    ivec2 uViewportDimensions;
};

out vec4 oColor;

// unused
vec3 viewPositionFromFragmentCoord()
{
    vec4 ndcPosition;
    ndcPosition.xy = 2.0 * gl_FragCoord.xy / uViewportDimensions - 1.0;
    ndcPosition.z = (2.0 * gl_FragCoord.z - gl_DepthRange.near - gl_DepthRange.far) / (gl_DepthRange.far - gl_DepthRange.near);
    ndcPosition.w = 1.0;

    vec4 clipPosition = ndcPosition / gl_FragCoord.w;

    return (uClipToViewMatrix * clipPosition).xyz;
}

vec4 attenuatedLightIntensity(in vec3 viewPosition, out vec3 directionToLight)
{
    vec3 differenceToLight = Light.viewPosition - viewPosition;
    float distanceToLightSquare = dot(differenceToLight, differenceToLight);
    directionToLight = differenceToLight * inversesqrt(distanceToLightSquare);
    return Light.intensity / (1.0 + Light.attenuation * sqrt(distanceToLightSquare));
}

void main()
{
    vec3 viewPosition = vec3(iViewPosition);
    vec3 viewNormal = normalize(iViewNormal);

    vec3 directionToLight;
    vec4 attenuatedLight = attenuatedLightIntensity(viewPosition, directionToLight);

    float incidenceAngleCos = dot(directionToLight, viewNormal);
    incidenceAngleCos = clamp(incidenceAngleCos, 0.0, 1.0);

    vec3 viewDirection = normalize(-viewPosition);

    vec3 halfAngleDirection = normalize(viewDirection + directionToLight);
    float halfAngle = acos(dot(viewNormal, halfAngleDirection));

    float gaussianExponent = halfAngle / Material.surfaceShininess;
    gaussianExponent = -(gaussianExponent * gaussianExponent);
    float gaussianTerm = exp(gaussianExponent);
    gaussianTerm = incidenceAngleCos != 0.0 ? gaussianTerm : 0.0;

    oColor = iColor * attenuatedLight * 0.75 * incidenceAngleCos
      + Material.specularColor * attenuatedLight * 0.25 * gaussianTerm
      + iColor * Light.ambientIntensity;
}
