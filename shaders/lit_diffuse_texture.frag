#version 330

layout(std140) uniform;

in Varyings
{
    vec3 viewPosition;
    vec3 viewNormal;
    vec2 texCoord;
} In;

uniform sampler2D _texture;

struct Light
{
    vec4 viewPosition;
    vec4 intensity;
};

const int LIGHT_COUNT = 4;

uniform LightBlock
{
    vec4 ambientIntensity;
    float attenuation;
    float maxIntensity;
    Light lights[LIGHT_COUNT];
} Lights;

out vec3 outColor;

float lightAttenuation(vec3 position, vec3 lightPosition, out vec3 directionToLight)
{
    vec3 offsetToLight = lightPosition - position;
    float distanceToLightSqr = dot(offsetToLight, offsetToLight);
    directionToLight = offsetToLight * inversesqrt(distanceToLightSqr);
    return 1.0 / (1.0 + Lights.attenuation * distanceToLightSqr);
}

vec3 calcDiffuseLighting(vec3 diffuseColor, Light light)
{
	vec3 viewNormal = normalize(In.viewNormal);

    vec3 directionToLight;
    vec3 lightIntensity;

    if (light.viewPosition.w == 0.0)
    {
        directionToLight = vec3(light.viewPosition);
        lightIntensity = vec3(light.intensity);
    }
    else
    {
        lightIntensity = vec3(light.intensity) * lightAttenuation(In.viewPosition, vec3(light.viewPosition), directionToLight);
    }

    float incidenceAngleCos = dot(viewNormal, directionToLight);
    incidenceAngleCos = clamp(incidenceAngleCos, 0.0, 1.0);
    return diffuseColor * lightIntensity * incidenceAngleCos;
}

void main()
{
    vec3 diffuseColor = texture(_texture, In.texCoord).rgb;
    vec3 accumulatedLight = diffuseColor * vec3(Lights.ambientIntensity);
    for (int index = 0; index < LIGHT_COUNT; ++index)
    {
        accumulatedLight += calcDiffuseLighting(diffuseColor, Lights.lights[index]);
    }
    outColor = accumulatedLight / Lights.maxIntensity;
}
