
#version 330 core

in vec3 FragPos;
in vec3 Normal;

uniform vec4 objectColor;

#define MAX_LIGHTS 10

uniform vec3 lightPos[MAX_LIGHTS];

uniform int numActiveLights;

out vec4 FragColor;

void main(){

    vec3 finalDiffuse = vec3(0.1);

    for (int i = 0; i < numActiveLights; ++i) {
        vec3 lightDir = normalize(lightPos[i] - FragPos);
        float diff =  max(dot(Normal, lightDir), 0.0);
        vec3 diffuse = objectColor.rgb * diff;
        finalDiffuse += diffuse;
    }

    FragColor = vec4(finalDiffuse, objectColor.a);
}