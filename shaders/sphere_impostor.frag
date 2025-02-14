#version 330

layout(std140) uniform;

smooth in vec2 iMapping;

out vec4 oColor;

uniform vec3 uSphereViewPosition;
uniform float uSphereRadius;
uniform vec4 uDiffuseColor;

uniform GammaCorrectionBlock
{
	float gammaInverse;
};

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

float attenuatedLightIntensity(in vec3 viewPosition, in vec3 viewLightPosition, out vec3 directionToLight)
{
    vec3 differenceToLight = viewLightPosition - viewPosition;
    float distanceToLightSquare = dot(differenceToLight, differenceToLight);
    directionToLight = differenceToLight * inversesqrt(distanceToLightSquare);
    return 1.0 / (1.0 + Lights.attenuation * distanceToLightSquare);
}

vec4 calcLighting(in Light light, in vec3 viewPosition, in vec3 viewNormal)
{
    vec3 directionToLight;
    vec4 lightIntensity;
    if (light.viewPosition.w == 0.0)
    {
        directionToLight = vec3(light.viewPosition);
        lightIntensity = light.intensity;
    }
    else
    {
        lightIntensity = light.intensity * attenuatedLightIntensity(viewPosition, light.viewPosition.xyz, directionToLight);
    }

    // Diffuse lighting
    float incidenceAngleCos = dot(directionToLight, viewNormal);
    incidenceAngleCos = clamp(incidenceAngleCos, 0.0, 1.0);

    // Specular lighting
    vec3 viewDirection = normalize(-viewPosition);
    vec3 halfAngleDirection = normalize(viewDirection + directionToLight);
    float halfAngle = acos(dot(viewNormal, halfAngleDirection));

    float gaussianExponent = halfAngle / Material.surfaceShininess;
    gaussianExponent = -(gaussianExponent * gaussianExponent);
    float gaussianTerm = exp(gaussianExponent);
    gaussianTerm = incidenceAngleCos != 0.0 ? gaussianTerm : 0.0;

    return uDiffuseColor * lightIntensity * incidenceAngleCos
      + Material.specularColor * lightIntensity * gaussianTerm;
}

void calcSphereValues(out vec3 viewPosition, out vec3 viewNormal)
{
    float distanceSquared = dot(iMapping, iMapping);
    if (distanceSquared > 1.0)
        discard;

    viewNormal = vec3(iMapping, sqrt(1.0 - distanceSquared));
    viewPosition = viewNormal * uSphereRadius + uSphereViewPosition;
}

void main()
{
    vec3 viewPosition;
    vec3 viewNormal;
    calcSphereValues(viewPosition, viewNormal);

    vec4 accumulatedLight = uDiffuseColor * Lights.ambientIntensity;
    for (int index = 0; index < LIGHT_COUNT; ++index)
    {
        accumulatedLight += calcLighting(Lights.lights[index], viewPosition, viewNormal);
    }
	accumulatedLight /= Lights.maxIntensity;
    oColor = pow(accumulatedLight, vec4(vec3(gammaInverse), 1.0));
}
