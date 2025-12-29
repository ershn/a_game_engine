#version 330

layout(std140) uniform;

in Varyings
{
    vec4 viewPosition;
    vec3 viewNormal;
    vec2 textureCoord;
} In;

uniform sampler2D _gaussianTexture;
uniform sampler2D _shininessTexture;
uniform bool _useShininessTex;

struct Light
{
    vec4 viewPosition;
    vec4 intensity;
};

const int LIGHT_COUNT = 2;

uniform LightBlock
{
    vec4 ambientIntensity;
    float attenuation;
    float maxIntensity;
    Light lights[LIGHT_COUNT];
} Lights;

uniform MaterialBlock
{
    vec4 diffuseColor;
    vec4 specularColor;
    float surfaceShininess;
} Material;

out vec4 outColor;

float attenuatedLightIntensity(in vec3 viewPosition, in vec3 viewLightPosition, out vec3 directionToLight)
{
    vec3 differenceToLight = viewLightPosition - viewPosition;
    float distanceToLightSquare = dot(differenceToLight, differenceToLight);
    directionToLight = differenceToLight * inversesqrt(distanceToLightSquare);
    return 1.0 / (1.0 + Lights.attenuation * distanceToLightSquare);
}

vec4 calcLighting(in Light light)
{
    vec3 viewPosition = vec3(In.viewPosition);
    vec3 viewNormal = normalize(In.viewNormal);

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
    float shininess;
    if (_useShininessTex)
		shininess = texture(_shininessTexture, In.textureCoord).r;
	else
        shininess = Material.surfaceShininess;

    vec3 viewDirection = normalize(-viewPosition);
    vec3 halfAngleDirection = normalize(viewDirection + directionToLight);
    float halfAngleCos = dot(viewNormal, halfAngleDirection);
    float gaussianTerm = texture(_gaussianTexture, vec2(sqrt(halfAngleCos), shininess)).r;
    // float halfAngle = acos(halfAngleCos);

    // float gaussianExponent = halfAngle / Material.surfaceShininess;
    // gaussianExponent = -(gaussianExponent * gaussianExponent);
    // float gaussianTerm = exp(gaussianExponent);
    gaussianTerm = incidenceAngleCos != 0.0 ? gaussianTerm : 0.0;

    return Material.diffuseColor * lightIntensity * incidenceAngleCos
      + Material.specularColor * lightIntensity * gaussianTerm;
}

void main()
{
    vec4 accumulatedLight = Material.diffuseColor * Lights.ambientIntensity;
    for (int index = 0; index < LIGHT_COUNT; ++index)
    {
        accumulatedLight += calcLighting(Lights.lights[index]);
    }
	accumulatedLight /= Lights.maxIntensity;
    outColor = accumulatedLight;
}
