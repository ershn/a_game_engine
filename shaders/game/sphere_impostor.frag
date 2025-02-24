#version 330

layout(std140) uniform;

in FragmentData {
    flat vec3 sphereViewPosition;
    flat float sphereRadius;
    vec2 mapping;
} In;

out vec4 outColor;

uniform GammaCorrectionBlock
{
	float gammaInverse;
};

uniform ProjectionBlock
{
    mat4 viewToClipMatrix;
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

struct Material
{
	vec4 diffuseColor;
	vec4 specularColor;
	float surfaceShininess;
};

uniform MaterialsBlock
{
    Material materials[4];
} Materials;

float attenuatedLightIntensity(in vec3 viewPosition, in vec3 viewLightPosition, out vec3 directionToLight)
{
    vec3 differenceToLight = viewLightPosition - viewPosition;
    float distanceToLightSquare = dot(differenceToLight, differenceToLight);
    directionToLight = differenceToLight * inversesqrt(distanceToLightSquare);
    return 1.0 / (1.0 + Lights.attenuation * distanceToLightSquare);
}

vec4 calcLighting(in Light light, in vec3 viewPosition, in vec3 viewNormal)
{
    Material material = Materials.materials[gl_PrimitiveID];

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

    float gaussianExponent = halfAngle / material.surfaceShininess;
    gaussianExponent = -(gaussianExponent * gaussianExponent);
    float gaussianTerm = exp(gaussianExponent);
    gaussianTerm = incidenceAngleCos != 0.0 ? gaussianTerm : 0.0;

    return material.diffuseColor * lightIntensity * incidenceAngleCos
      + material.specularColor * lightIntensity * gaussianTerm;
}

float square(float x)
{
    return x * x;
}

void rayTraceSphereValues(out vec3 viewPosition, out vec3 viewNormal)
{
    vec3 rayDirection = normalize(vec3(In.mapping * In.sphereRadius, 0.0) + In.sphereViewPosition);
    float ds = dot(rayDirection, In.sphereViewPosition);
    float discriminant = 4.0 * (square(ds) - dot(In.sphereViewPosition, In.sphereViewPosition) + square(In.sphereRadius));
    if (discriminant < 0.0)
        discard;

    float rootTerm = sqrt(discriminant) / 2.0;
    viewPosition = min(ds + rootTerm, ds - rootTerm) * rayDirection;
    viewNormal = normalize(viewPosition - In.sphereViewPosition);
}

void main()
{
    vec3 viewPosition;
    vec3 viewNormal;
    rayTraceSphereValues(viewPosition, viewNormal);

    vec4 accumulatedLight = Materials.materials[gl_PrimitiveID].diffuseColor * Lights.ambientIntensity;
    for (int index = 0; index < LIGHT_COUNT; ++index)
    {
        accumulatedLight += calcLighting(Lights.lights[index], viewPosition, viewNormal);
    }
	accumulatedLight /= Lights.maxIntensity;

    outColor = pow(accumulatedLight, vec4(vec3(gammaInverse), 1.0));

    vec4 clipPosition = viewToClipMatrix * vec4(viewPosition, 1.0);
    float ndcZ = clipPosition.z / clipPosition.w;
    gl_FragDepth = (ndcZ * gl_DepthRange.diff + gl_DepthRange.near + gl_DepthRange.far) / 2.0;
}
