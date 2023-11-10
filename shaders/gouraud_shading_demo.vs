#version 330 core

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;

// Transformation matrices.
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
uniform vec3 ambientLight;
uniform vec3 dirLightDir;
uniform vec3 dirLightRadiance;
uniform vec3 pointLightPos;
uniform vec3 pointLightIntensity;

// Data pass to fragment shader.
out vec3 iLightingColor;


vec3 Diffuse(vec3 Kd, vec3 I, vec3 N, vec3 lightDir)
{
    return Kd * I * max(0, dot(N, lightDir));
}

vec3 Specular( /* Put the parameters here. */ )
{
    // Try to implement yourself!
    return vec3(0.0, 0.0, 0.0);
}

void main()
{
    gl_Position = MVP * vec4(Position, 1.0);
    
    // Compute vertex lighting in view space.
    vec4 tmpPos = viewMatrix * worldMatrix * vec4(Position, 1.0);
    vec3 vsPosition = tmpPos.xyz / tmpPos.w;
    vec3 vsNormal = (normalMatrix * vec4(Normal, 0.0)).xyz;
    vsNormal = normalize(vsNormal);
    // -------------------------------------------------------------
    // Ambient light.
    vec3 ambient = Ka * ambientLight;
    // -------------------------------------------------------------
    // Directional light.
    vec3 vsLightDir = (viewMatrix * vec4(-dirLightDir, 0.0)).xyz;
    vsLightDir = normalize(vsLightDir);
    // Diffuse.
    vec3 diffuse = Diffuse(Kd, dirLightRadiance, vsNormal, vsLightDir);
    // Specular.
    vec3 specular = Specular();
    vec3 dirLight = diffuse + specular;
    // -------------------------------------------------------------
    // Point light.
    tmpPos = viewMatrix * vec4(pointLightPos, 1.0);
    vec3 vsLightPos = tmpPos.xyz / tmpPos.w;
    vsLightDir = normalize(vsLightPos - vsPosition);
    float distSurfaceToLight = distance(vsLightPos, vsPosition);
    float attenuation = 1.0f / (distSurfaceToLight * distSurfaceToLight);
    vec3 radiance = pointLightIntensity * attenuation;
    // Diffuse.
    diffuse = Diffuse(Kd, radiance, vsNormal, vsLightDir);
    // Specular.
    specular = Specular();
    vec3 pointLight = diffuse + specular;
    // -------------------------------------------------------------
    
    iLightingColor = ambient + dirLight + pointLight;
}