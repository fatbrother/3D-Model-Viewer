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

vec3 Diffuse(vec3 Kd, vec3 I, vec3 N, vec3 L)
{
    return Kd * I * max(0, dot(N, L));
}

vec3 Specular(vec3 Ks, vec3 I, vec3 R, vec3 E, float Ns)
{
    return Ks * I * pow(max(0, dot(R, E)), Ns);
}

void main()
{
    vec3 L = normalize(iPointLightPos - iPosition);
    vec3 E = normalize(-iPosition);
    vec3 R = 2 * dot(L, iNormal) * iNormal - L;

    // Ambient light.
    vec3 ambient = Ambient(iKa, iAmbientLight);

    // Directional light.
    vec3 DirLight = Diffuse(iKd, iDirLightRadiance, iNormal, iDirLightDir);
    DirLight += Specular(iKs, iDirLightRadiance, R, E, iNs);

    // Point light.
    vec3 PointLight = Diffuse(iKd, iPointLightIntensity, iNormal, L);
    PointLight += Specular(iKs, iPointLightIntensity, R, E, iNs);

    FragColor = vec4(ambient + DirLight + PointLight, 1.0);
}
