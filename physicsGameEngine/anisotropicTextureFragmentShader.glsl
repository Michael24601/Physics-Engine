
#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec3 Tangent;
in vec3 Bitangent;
in vec2 TexCoord;

uniform sampler2D textureSampler;

#define MAX_LIGHTS 10

uniform vec3 lightPos[MAX_LIGHTS];
uniform vec4 lightColors[MAX_LIGHTS];

uniform int numActiveLights;

// Anisotropic shading parameters
uniform vec3 viewPos;
uniform float roughness;
uniform float fresnel;

out vec4 FragColor;

void main() {
    vec3 finalDiffuse = vec3(0.0);
    vec3 finalSpecular = vec3(0.0);

    vec3 texColor = texture(textureSampler, TexCoord).rgb;

    for (int i = 0; i < numActiveLights; ++i) {

        // Diffuse calculation
        vec3 lightDir = normalize(lightPos[i] - FragPos);
        float diff = max(dot(Normal, lightDir), 0.0);
        vec3 diffuse = texColor * diff;
        finalDiffuse += diffuse;

        // Anisotropic specular calculation
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 halfwayDir = normalize(lightDir + viewDir);
        vec3 reflectDir = reflect(-lightDir, Normal);
        vec3 h = normalize(viewDir + lightDir);
        float NdotH = max(dot(Normal, halfwayDir), 0.0);
        float D = (roughness * roughness) / (3.14159265359 * pow((dot(h, Tangent) 
            * dot(h, Bitangent) * (roughness * roughness - 1.0) + 1.0), 2.0));
        float G = min(
            1.0,
            min(
                2.0 * NdotH * dot(Normal, viewDir) / dot(viewDir, halfwayDir), 
                2.0 * NdotH * dot(Normal, lightDir) / dot(lightDir, halfwayDir)
            )
        );
        float F = fresnel + (1.0 - fresnel) * pow(1.0 - NdotH, 5.0);

        /*
            The opacity of the anisotropic shading effect is determined
            through the alpha value
        */
        vec3 specular = lightColors[i].rgb * (D * G * F) 
            / (4.0 * dot(Normal, viewDir) * dot(Normal, lightDir));
        finalSpecular += specular;
    }

    // 0.1 looks best
    vec3 ambientColor = 0.1 * texColor;
    vec3 resultColor = finalDiffuse + finalSpecular + ambientColor;

    FragColor = vec4(resultColor, 1.0);
}