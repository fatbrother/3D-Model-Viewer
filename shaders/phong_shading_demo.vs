#version 330 core

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;

// Transformation matrix.
uniform mat4 worldMatrix;
uniform mat4 viewMatrix;
uniform mat4 normalMatrix;
uniform mat4 MVP;

// Material properties.
uniform vec3 Ka;
uniform vec3 Kd;
uniform vec3 Ks;
uniform float Ns;
// Light data.
uniform vec3 dirLightDir;
uniform vec3 dirLightRadiance;
uniform vec3 pointLightPos;
uniform vec3 pointLightIntensity;
uniform vec3 spotLightPos;
uniform vec3 spotLightDir;
uniform vec3 spotLightIntensity;
uniform float spotLightCutoff;
uniform float spotLightTotalWidth;
uniform vec3 ambientLight;

// Data pass to fragment shader.
out vec3 vPosition;
out vec3 vNormal;
out vec3 vColor;

vec3 Ambient(vec3 Ka, vec3 I)
{
    return Ka * I;
}

vec3 Diffuse(vec3 Kd, vec3 I, vec3 N, vec3 lightDir)
{
    return Kd * I * max(0, dot(N, lightDir));
}

vec3 Specular(vec3 Ks, vec3 I, vec3 L, vec3 N, vec3 P, float shininess)
{
    vec3 E = normalize(-P);
    vec3 H = normalize(L + E);
    return Ks * I * pow(max(0, dot(N, H)), shininess);
}

void main()
{
    gl_Position = MVP * vec4(Position, 1.0);

    vPosition = vec3(viewMatrix * worldMatrix * vec4(Position, 1.0));
    vNormal = normalize(vec3(normalMatrix * vec4(Normal, 0.0)));
  
    vec3 vDirLightDir = vec3(viewMatrix * vec4(dirLightDir, 0.0));
    vDirLightDir = normalize(vDirLightDir);

    vec3 vPointLightPos = vec3(viewMatrix * vec4(pointLightPos, 1.0));
    vec3 pointLightDist = vPointLightPos - vPosition;
    float pointLightDistSqr = dot(pointLightDist, pointLightDist);
    vec3 vPointLightIntensity = pointLightIntensity / pointLightDistSqr;

    vec3 vSpotLightPos = vec3(viewMatrix * vec4(spotLightPos, 1.0));
    vec3 vSpotLightDir = vec3(viewMatrix * vec4(spotLightDir, 0.0));
    vSpotLightDir = normalize(vSpotLightDir);
    vec3 spotLightDist = vSpotLightPos - vPosition;
    float spotLightDistSqr = dot(spotLightDist, spotLightDist);
    float deltaDeg = degrees(acos(dot(normalize(spotLightDist), -normalize(vSpotLightDir))));
    float factor = 0.0;
    if (deltaDeg < spotLightCutoff)
    {
        factor = 1.0;
    }
    else if (deltaDeg < spotLightTotalWidth)
    {
        factor = (deltaDeg - spotLightTotalWidth) / (spotLightCutoff - spotLightTotalWidth);
    }
    vec3 vSpotLightIntensity = spotLightIntensity * factor / spotLightDistSqr;

    // Ambient light.
    vec3 ambient = Ambient(Ka, ambientLight);

    // Directional light.
    vec3 dirLight = Diffuse(Kd, dirLightRadiance, vNormal, vDirLightDir);
    dirLight += Specular(Ks, dirLightRadiance, vDirLightDir, vNormal, vPosition, Ns);

    // Point light.
    vec3 L = normalize(vPointLightPos - vPosition);
    vec3 pointLight = Diffuse(Kd, vPointLightIntensity, vNormal, L);
    pointLight += Specular(Ks, vPointLightIntensity, L, vNormal, vPosition, Ns);

    // Spot light.
    vec3 S = normalize(vSpotLightPos - vPosition);
    vec3 spotLight = Diffuse(Kd, vSpotLightIntensity, vNormal, S);
    spotLight += Specular(Ks, vSpotLightIntensity, S, vNormal, vPosition, Ns);

    vColor = ambient + dirLight + pointLight + spotLight;
}