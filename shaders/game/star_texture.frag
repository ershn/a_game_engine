#version 330

layout(std140) uniform;

in Varyings
{
    vec3 viewPosition;
    vec3 viewNormal;
    vec2 texCoord;
} In;

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

vec3 calcDiffuseLighting(vec3 diffuseColor, vec3 viewNormal, Light light)
{
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

vec3 calcLighting(vec3 diffuseColor, vec3 viewNormal)
{
    vec3 accumulatedLight = diffuseColor * vec3(Lights.ambientIntensity);
    for (int index = 0; index < LIGHT_COUNT; ++index)
    {
        accumulatedLight += calcDiffuseLighting(diffuseColor, viewNormal, Lights.lights[index]);
    }
    return accumulatedLight / Lights.maxIntensity;
}

const float PI = acos(-1.0);

const float STAR_POINT_COUNT = 5.0;
const float STAR_SIDE_ANGLE = 2 * PI / STAR_POINT_COUNT;
const float STAR_MIN_RADIUS = 0.07;
const float STAR_MAX_RADIUS = 0.2;
const vec2 STAR_CENTER = vec2(0.5, 0.5);

const vec2 P0 = vec2(STAR_MAX_RADIUS, 0.0);
const vec2 P1 = vec2(cos(0.5), sin(0.5)) * STAR_MIN_RADIUS;

const vec3 STAR_COLOR = vec3(1.0, 0.5161, 0.0);
const vec3 BACKGROUND_COLOR = vec3(0.0, 0.0, 1.0);

void main()
{
    vec2 p = In.texCoord - STAR_CENTER;
    float angle = mod(atan(p.y, p.x), STAR_SIDE_ANGLE) / STAR_SIDE_ANGLE;
    angle = angle > 0.5 ? 1.0 - angle : angle;
    vec2 pi = vec2(cos(angle), sin(angle)) * length(p);

    vec2 d1 = P1 - P0;
    vec2 di = pi - P0;
    float t = step(0.0, cross(vec3(d1, 0.0), vec3(di, 0.0)).z);
    vec3 diffuseColor = mix(BACKGROUND_COLOR, STAR_COLOR, t);

    outColor = calcLighting(diffuseColor, normalize(In.viewNormal));
}
