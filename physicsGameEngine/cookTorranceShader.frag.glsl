
#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform vec4 color;
uniform sampler2D objectTexture;
uniform bool useTexture;

#define MAX_LIGHTS 10

uniform vec3 lightPos[MAX_LIGHTS];
uniform vec4 lightColors[MAX_LIGHTS];

uniform int numActiveLights;

// Cook-Torrance parameters
uniform float roughness;
uniform float fresnel;

uniform mat4 view;

out vec4 FragColor;

void main(){

    // When we disable face culling in order to draw both sides
    // of the faces of an object (such as when drawing cloth),
    // not only is the front face drawn, but also the back face
    // (which is the same face but with the vertices in reverse
    // order). The noraml of said face is the same as the front
    // face but inverted.
    vec3 normal = gl_FrontFacing ? Normal : -Normal;

    // We can extract the camera position from thew view matrix, 
    // no need to send it as a uniform
    vec3 viewPos = (inverse(view))[3].xyz;

    vec3 finalDiffuse = vec3(0.0);
    vec3 finalSpecular = vec3(0.0);

    vec4 objColor;
    if(useTexture)
        objColor = texture(objectTexture, TexCoord);
    else
        objColor = color;

    for (int i = 0; i < numActiveLights; ++i) {
        // Diffuse calculation
        vec3 lightDir = normalize(lightPos[i] - FragPos);
        float diff = max(dot(Normal, lightDir), 0.0);
        vec3 diffuse = objColor.rgb * diff;
        finalDiffuse += diffuse;

        // Cook-Torrance specular calculation
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float NdotH = max(dot(normal, halfwayDir), 0.0);
        float D = (roughness * roughness) / (3.14159265359 * pow(
            (NdotH * NdotH * (roughness * roughness - 1.0) + 1.0), 
            2.0)
        );
        float G = min(
            1.0, 
            min(
                2.0 * NdotH * dot(normal, viewDir) / dot(viewDir, halfwayDir), 
                2.0 * NdotH * dot(normal, lightDir) / dot(lightDir, halfwayDir)
            )
        );
        float F = fresnel + (1.0 - fresnel) * pow(1.0 - NdotH, 5.0);

        /* 
            The opacity of the Cook-Torrance effect is determined through
            the alpha value.
        */
        vec3 specular = lightColors[i].rgb * (D * G * F) 
            / (4.0 * dot(normal, viewDir) * dot(normal, lightDir));
        finalSpecular += specular;
    }

    // 0.1 looks best
    vec3 ambientColor = 0.1 * objColor.rgb;
    vec3 resultColor = finalDiffuse + finalSpecular + ambientColor;

    FragColor = vec4(resultColor, objColor.a);
}