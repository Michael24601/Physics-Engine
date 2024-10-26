
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

uniform mat4 view;

// Specular lighting parameters
uniform float shininess;

out vec4 FragColor;

void main(){

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

        // Specular calculation
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, Normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);

        // The opacity of the phong effect is determined through the alpha value
        vec3 specular = lightColors[i].rgb * spec * lightColors[i].a;
        finalSpecular += specular;
    }

    // 0.1 looks best
    vec3 ambientColor = 0.1 * objColor.rgb;
    vec3 resultColor = finalDiffuse + finalSpecular + ambientColor;

    FragColor = vec4(resultColor, objColor.a);
}