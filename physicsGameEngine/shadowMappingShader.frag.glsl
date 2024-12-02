
#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
in vec3 ViewPos;
in vec4 FragPosLightSpace;

uniform vec4 color;
uniform sampler2D objectTexture;
uniform bool useTexture;

uniform vec3 lightPos;

uniform bool PCF;

uniform float shadowStrength;

uniform sampler2D shadowMap;

out vec4 FragColor;

// 2D shadow map calculation
float ShadowCalculation2D(vec4 fragPosLightSpace, vec3 lightDir) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;

    float bias = max(0.05 * (1.0 - dot(Normal, lightDir)), 0.05);
    return currentDepth - bias > closestDepth ? shadowStrength : 0.0;
}


// 2D shadow map PCF
float PCFShadowCalculation2D(vec4 fragPosLightSpace, vec3 lightDir) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    float currentDepth = projCoords.z;
    float bias = max(0.05 * (1.0 - dot(Normal, lightDir)), 0.05);

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? shadowStrength : 0.0;
        }
    }
    return shadow / 9.0;
}


void main() {
    vec3 normal = gl_FrontFacing ? Normal : -Normal;
    vec4 objectColor = useTexture ? texture(objectTexture, TexCoord) : color;

    vec3 lightColor = vec3(1.0);
    vec3 ambient = 0.15 * lightColor;

    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 viewDir = normalize(ViewPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;

    float shadow = PCF ? PCFShadowCalculation2D(FragPosLightSpace, lightDir)
        : ShadowCalculation2D(FragPosLightSpace, lightDir);

    if (shadow > 0) {
        specular = vec3(0.0);
    }

    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * objectColor.rgb;
    FragColor = vec4(lighting, 1.0);
}