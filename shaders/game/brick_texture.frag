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

const vec3 BRICK_COLOR = vec3(0.5, 0.15, 0.14);
const vec3 MORTAR_COLOR = vec3(0.5, 0.5, 0.5);

const float BRICK_WIDTH = 0.25;
const float BRICK_HEIGHT = 0.09;
const float MORTAR_SIZE = 0.01;

const float CELL_WIDTH = BRICK_WIDTH + MORTAR_SIZE;
const float CELL_HEIGHT = BRICK_HEIGHT + MORTAR_SIZE;

const float MORTAR_N_HALF_WIDTH = MORTAR_SIZE * 0.5 / CELL_WIDTH;
const float MORTAR_N_HALF_HEIGHT = MORTAR_SIZE * 0.5 / CELL_HEIGHT;

vec3 bumpMap(vec3 position, float bumpHeight)
{
    vec3 px = dFdx(position);
    vec3 py = dFdy(position);
    vec3 normal = cross(px, py);
    vec3 perturbationX = dFdx(bumpHeight) * cross(normal, py);
    vec3 perturbationY = dFdy(bumpHeight) * cross(px, normal);
    vec3 perturbation = (perturbationX + perturbationY) / length(normal);
    return normal + perturbation;
}

vec3 bumpMapNormal(vec3 position, vec3 normal, float bumpHeight)
{
    vec3 bumpedPosition = position + normalize(normal) * bumpHeight;
    return cross(dFdx(bumpedPosition), dFdy(bumpedPosition));
}

void main()
{
    float sCells = In.texCoord.s / CELL_WIDTH;
    float tCells = In.texCoord.t / CELL_HEIGHT;

    if (mod(tCells, 2.0) >= 1.0)
        sCells += 0.5;

    float sCell = sCells - floor(sCells);
    float tCell = tCells - floor(tCells);

    float sIsBrick = step(MORTAR_N_HALF_WIDTH, sCell) - step(1.0 - MORTAR_N_HALF_WIDTH, sCell);
    float tIsBrick = step(MORTAR_N_HALF_HEIGHT, tCell) - step(1.0 - MORTAR_N_HALF_HEIGHT, tCell);

    vec3 diffuseColor = mix(MORTAR_COLOR, BRICK_COLOR, sIsBrick * tIsBrick);

    float sBump = smoothstep(0.0, MORTAR_N_HALF_WIDTH, sCell) - smoothstep(1.0 - MORTAR_N_HALF_WIDTH, 1.0, sCell);
    float tBump = smoothstep(0.0, MORTAR_N_HALF_HEIGHT, tCell) - smoothstep(1.0 - MORTAR_N_HALF_HEIGHT, 1.0, tCell);
    float stBump = sBump * tBump;
    stBump *= 0.0025;

    outColor = calcLighting(diffuseColor, normalize(bumpMapNormal(In.viewPosition, In.viewNormal, stBump)));
}
