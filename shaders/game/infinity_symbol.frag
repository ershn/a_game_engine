#version 330

layout(std140) uniform;

in VertexData
{
    vec4 viewPosition;
    vec3 viewNormal;
    vec2 textureCoord;
} In;

out vec4 oColor;

uniform sampler2D uGaussianTexture;
uniform sampler2D uShininessTexture;
uniform bool uUseShininessTex;

uniform GammaCorrectionBlock
{
	float gammaInverse;
};

struct Light
{
    vec4 viewPosition;
    vec4 intensity;
};

const int LIGHT_COUNT = 2;

uniform LightsBlock
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
    if (uUseShininessTex)
		shininess = texture(uShininessTexture, In.textureCoord).r;
	else
        shininess = Material.surfaceShininess;

    vec3 viewDirection = normalize(-viewPosition);
    vec3 halfAngleDirection = normalize(viewDirection + directionToLight);
    float halfAngleCos = dot(viewNormal, halfAngleDirection);
    float gaussianTerm = texture(uGaussianTexture, vec2(halfAngleCos, shininess)).r;
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
    oColor = pow(accumulatedLight, vec4(vec3(gammaInverse), 1.0));
}
