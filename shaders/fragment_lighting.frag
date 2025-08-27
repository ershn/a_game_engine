#version 330

layout(std140) uniform;

smooth in vec4 iViewPosition;
smooth in vec4 iDiffuseColor;
smooth in vec3 iViewNormal;

uniform sampler1D uGaussianTexture;

struct Light
{
    vec4 viewPosition;
    vec4 intensity;
};

const int LIGHT_COUNT = 4;

uniform LightsBlock
{
    vec4 ambientIntensity;
    float attenuation;
    float maxIntensity;
    Light lights[LIGHT_COUNT];
} Lights;

uniform MaterialBlock
{
    vec4 specularColor;
    float surfaceShininess;
} Material;

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

float lightAttenuation(in vec3 viewPosition, in vec3 lightViewPosition, out vec3 directionToLight)
{
    vec3 differenceToLight = lightViewPosition - viewPosition;
    float distanceToLightSquare = dot(differenceToLight, differenceToLight);
    directionToLight = differenceToLight * inversesqrt(distanceToLightSquare);
    return 1.0 / (1.0 + Lights.attenuation * distanceToLightSquare);
}

vec4 calcLighting(in Light light)
{
    vec3 viewPosition = vec3(iViewPosition);
    vec3 viewNormal = normalize(iViewNormal);

    vec3 directionToLight;
    vec4 lightIntensity;
    if (light.viewPosition.w == 0.0)
    {
        directionToLight = vec3(light.viewPosition);
        lightIntensity = light.intensity;
    }
    else
    {
        lightIntensity = light.intensity * lightAttenuation(viewPosition, light.viewPosition.xyz, directionToLight);
    }

    // Diffuse lighting
    float incidenceAngleCos = dot(directionToLight, viewNormal);
    incidenceAngleCos = clamp(incidenceAngleCos, 0.0, 1.0);

    // Specular lighting
    vec3 viewDirection = normalize(-viewPosition);
    vec3 halfAngleDirection = normalize(viewDirection + directionToLight);
    float halfAngleCos = dot(viewNormal, halfAngleDirection);
    float gaussianTerm = texture(uGaussianTexture, halfAngleCos).r;
    // float halfAngle = acos(halfAngleCos);

    // float gaussianExponent = halfAngle / Material.surfaceShininess;
    // gaussianExponent = -(gaussianExponent * gaussianExponent);
    // float gaussianTerm = exp(gaussianExponent);
    gaussianTerm = incidenceAngleCos != 0.0 ? gaussianTerm : 0.0;

    return iDiffuseColor * lightIntensity * incidenceAngleCos
      + Material.specularColor * lightIntensity * gaussianTerm;
}

void main()
{
    vec4 accumulatedLight = iDiffuseColor * Lights.ambientIntensity;
    for (int index = 0; index < LIGHT_COUNT; ++index)
    {
        accumulatedLight += calcLighting(Lights.lights[index]);
    }
	accumulatedLight /= Lights.maxIntensity;
    oColor = accumulatedLight;
}
