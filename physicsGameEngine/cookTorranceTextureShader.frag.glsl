
 #version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform sampler2D objectTexture;

#define MAX_LIGHTS 10

uniform vec3 lightPos[MAX_LIGHTS];
uniform vec4 lightColors[MAX_LIGHTS];

uniform int numActiveLights;

uniform float roughness;  
uniform float fresnel;   

uniform mat4 view;

out vec4 FragColor;

void main(){

    // We can extract the camera position from thew view matrix, 
    // no need to send it as a uniform
    vec3 viewPos = (inverse(view))[3].xyz;

    vec3 finalDiffuse = vec3(0.0);
    vec3 finalSpecular = vec3(0.0);

    for (int i = 0; i < numActiveLights; ++i) {
        vec3 lightDir = normalize(lightPos[i] - FragPos);
        float diff = max(dot(Normal, lightDir), 0.0);
        vec3 diffuse = texture(objectTexture, TexCoord).rgb * diff;
        finalDiffuse += diffuse;

        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float NdotH = max(dot(Normal, halfwayDir), 0.0);
        float D = (roughness * roughness) / (3.14159265359 * pow(
            (NdotH * NdotH * (roughness * roughness - 1.0) + 1.0), 
            2.0
        ));
        float G = min(
            1.0, min(
                2.0 * NdotH * dot(Normal, viewDir) / dot(viewDir, halfwayDir), 
                2.0 * NdotH * dot(Normal, lightDir) / dot(lightDir, halfwayDir)
            )
        );
        float F = fresnel + (1.0 - fresnel) * pow(1.0 - NdotH, 5.0);

        vec3 specular = lightColors[i].rgb * (D * G * F) 
            / (4.0 * dot(Normal, viewDir) * dot(Normal, lightDir));
        finalSpecular += specular;
    }

    vec3 ambientColor = 0.1 * texture(objectTexture, TexCoord).rgb;
    vec3 resultColor = finalDiffuse + finalSpecular + ambientColor;

    FragColor = vec4(resultColor, 1.0);
}