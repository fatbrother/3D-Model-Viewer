#version 330 core

// Data from vertex shader.
in vec3 iPosition;
in vec3 iNormal;

// Material properties.
in vec3 iKa;
in vec3 iKd;
in vec3 iKs;
in float iNs;

// Light data.
in vec3 iDirLightDir;
in vec3 iDirLightRadiance;
in vec3 iPointLightPos;
in vec3 iPointLightIntensity;
in vec3 iAmbientLight;

out vec4 FragColor;

vec3 Ambient(vec3 Ka, vec3 I)
{
    return Ka * I;
}

vec3 Diffuse(vec3 Kd, vec3 I, vec3 N, vec3 lightDir)
{
    return Kd * I * max(0, dot(N, lightDir));
}

vec3 Specular(vec3 Ks, vec3 I, vec3 lightDir, vec3 N, float shininess)
{
    vec3 E = normalize(-iPosition);
    vec3 L = normalize(lightDir);
    vec3 H = normalize(L + E);
    return Ks * I * pow(max(0, dot(N, H)), shininess);
}

void main()
{
    // Ambient light.
    vec3 ambient = Ambient(iKa, iAmbientLight);

    // Directional light.
    vec3 dirLight = Diffuse(iKd, iDirLightRadiance, iNormal, iDirLightDir);
    dirLight += Specular(iKs, iDirLightRadiance, iDirLightDir, iNormal, iNs);

    // Point light.
    vec3 L = normalize(iPointLightPos - iPosition);
    vec3 pointLight = Diffuse(iKd, iPointLightIntensity, iNormal, L);
    pointLight += Specular(iKs, iPointLightIntensity, L, iNormal, iNs);

    FragColor = vec4(ambient + dirLight + pointLight, 1);
}
