#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform sampler2D objectNormalTexture;
uniform sampler2D objectAOTexture;
uniform sampler2D objectRoughnessTexture;
uniform sampler2D objectDiffuseTexture;
uniform sampler2D objectBaseColorTexture;
uniform sampler2D opacityTexture;

#define MAX_LIGHTS 10

uniform vec3 lightPos[MAX_LIGHTS];

uniform bool noLight;

uniform int numActiveLights;

out vec4 FragColor;

void main() {


    // If the opacity texture says its transparent
    vec4 texOpacity = texture(opacityTexture, TexCoord);
    if(texOpacity.r < 0.1){
        discard;
    }

    if(noLight){  
        FragColor = texture(objectBaseColorTexture, TexCoord);
        return;
    }

    vec3 finalDiffuse = vec3(0.1);

    for (int i = 0; i < numActiveLights; ++i) {
        vec3 lightDir = normalize(lightPos[i] - FragPos);
        float diff = max(dot(Normal, lightDir), 0.0);
        finalDiffuse += diff;
    }

    vec4 baseColor = texture(objectBaseColorTexture, TexCoord);
    float ao = texture(objectAOTexture, TexCoord).r;
    float roughness = texture(objectRoughnessTexture, TexCoord).r;
    vec4 diffuse = texture(objectDiffuseTexture, TexCoord);
    vec3 normal = texture(objectNormalTexture, TexCoord).rgb * 2.0 - 1.0;

    // Combine textures
    vec3 finalColor = baseColor.rgb * (1.0 - ao) + diffuse.rgb * ao;
    finalColor *= finalDiffuse;

    // Apply normal mapping
    vec3 N = normalize(normal);
    vec3 L = normalize(lightPos[0] - FragPos);
    float NdotL = max(dot(N, L), 0.0);
    finalColor *= NdotL;

    FragColor = vec4(finalColor, 1.0);
}
