#version 330 core

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
uniform sampler2D mapKd;
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

// Camera position.
uniform vec3 locCameraPos;

in vec3 fPosition;
in vec3 fNormal;
in vec2 fTexCoord;

out vec4 FragColor;

vec3 Ambient(vec3 Ka, vec3 I)
{
    return Ka * I;
}

vec3 Diffuse(vec3 texColor, vec3 I, vec3 N, vec3 lightDir)
{
    return texColor * I * max(0, dot(N, lightDir));
}

vec3 Specular(vec3 Ks, vec3 I, vec3 L, vec3 N, vec3 E, float shininess)
{
    vec3 H = normalize(L + E);
    return Ks * I * pow(max(0, dot(N, H)), shininess);
}

void main()
{
    vec3 vDirLightDir = vec3(viewMatrix * vec4(dirLightDir, 0.0));
    vDirLightDir = normalize(vDirLightDir);

    vec3 vPointLightPos = vec3(viewMatrix * vec4(pointLightPos, 1.0));
    vec3 pointLightDist = vPointLightPos - fPosition;
    float pointLightDistSqr = dot(pointLightDist, pointLightDist);
    vec3 vPointLightIntensity = pointLightIntensity / pointLightDistSqr;

    vec3 vSpotLightPos = vec3(viewMatrix * vec4(spotLightPos, 1.0));
    vec3 vSpotLightDir = vec3(viewMatrix * vec4(spotLightDir, 0.0));
    vSpotLightDir = normalize(vSpotLightDir);
    vec3 spotLightDist = vSpotLightPos - fPosition;
    float spotLightDistSqr = dot(spotLightDist, spotLightDist);
    float deltaDeg = degrees(acos(dot(normalize(spotLightDist), -normalize(vSpotLightDir))));
    float factor = clamp((spotLightTotalWidth - deltaDeg) / spotLightCutoff, 0, 1);
    vec3 vSpotLightIntensity = spotLightIntensity * factor / spotLightDistSqr;

    // Ambient light.
    vec3 ambient = Ambient(Ka, ambientLight);

    // Eye vector.
    vec3 E = normalize(locCameraPos - fPosition);

    // Texture color.
    vec3 texColor = texture2D(mapKd, fTexCoord).rgb;

    // Directional light.
    vec3 dirLight = Diffuse(texColor, dirLightRadiance, fNormal, vDirLightDir);
    dirLight += Specular(Ks, dirLightRadiance, vDirLightDir, fNormal, E, Ns);

    // Point light.
    vec3 P = normalize(vPointLightPos - fPosition);
    vec3 pointLight = Diffuse(texColor, vPointLightIntensity, fNormal, P);
    pointLight += Specular(Ks, vPointLightIntensity, P, fNormal, E, Ns);

    // Spot light.
    vec3 S = normalize(vSpotLightPos - fPosition);
    vec3 spotLight = Diffuse(texColor, vSpotLightIntensity, fNormal, S);
    spotLight += Specular(Ks, vSpotLightIntensity, S, fNormal, E, Ns);

    FragColor = vec4(ambient + dirLight + pointLight + spotLight, 1.0);
}
