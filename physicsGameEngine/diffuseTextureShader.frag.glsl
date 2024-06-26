#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform sampler2D objectTexture;

#define MAX_LIGHTS 10

uniform vec3 lightPos[MAX_LIGHTS];
uniform vec4 lightColors[MAX_LIGHTS];
uniform int numActiveLights;

out vec4 FragColor;

void main(){
    vec3 finalDiffuse = vec3(0.1);
    vec3 ambient = vec3(0.2);

    for (int i = 0; i < numActiveLights; ++i) {
        vec3 lightDir = normalize(lightPos[i] - FragPos);
        float diff = max(dot(Normal, lightDir), 0.0);
        finalDiffuse += diff * lightColors[i].rgb;
    }

    vec4 texColor = texture(objectTexture, TexCoord);

    // This just ensures that when the texture is transparent
    // no color is rendered
    if(texColor.a < 0.2)
        discard;

    FragColor = vec4(texColor.rgb * (finalDiffuse + ambient), texColor.a);
}
