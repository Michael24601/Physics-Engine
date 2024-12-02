
#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
in vec3 ViewPos;

uniform vec4 color;
uniform sampler2D objectTexture;
uniform bool useTexture;

uniform vec3 lightPos;

uniform bool PCF;

uniform float shadowStrength;

uniform samplerCube shadowCubemap;
uniform float farPlane;

out vec4 FragColor;

vec3 sampleOffsetDirections[20] = vec3[](
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
); 

float ShadowCalculationCubemap(vec3 fragPos){
    // get vector between fragment position and light position
    vec3 fragToLight = fragPos - lightPos;
    // use the light to fragment vector to sample from the depth map    
    float closestDepth = texture(shadowCubemap, fragToLight).r;
    // it is currently in linear range between [0,1]. Re-transform back to original value
    closestDepth *= farPlane;
    // now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);
    // now test for shadows
    float bias = 0.05; 
    float shadow = currentDepth -  bias > closestDepth ? shadowStrength : 0.0;

    return shadow;
}  

// Cubemap shadow map PCF
float PCFShadowCalculationCubemap(vec3 fragPos){
    // get vector between fragment position and light position
    vec3 fragToLight = fragPos - lightPos;
    // now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);
    float shadow = 0.0;
    float bias   = 0.15;
    int samples  = 20;
    float viewDistance = length(ViewPos - fragPos);
    float diskRadius = (1.0 + (viewDistance / farPlane)); 
    for(int i = 0; i < samples; ++i) {
        float closestDepth = texture(shadowCubemap, fragToLight + sampleOffsetDirections[i] * diskRadius).r;
        closestDepth *= farPlane;   // undo mapping [0;1]
        if(currentDepth - bias > closestDepth)
            shadow += shadowStrength;
    }
    shadow /= float(samples);  
    return shadow;
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

    float shadow = PCF ? PCFShadowCalculationCubemap(FragPos)
                     : ShadowCalculationCubemap(FragPos);

    if (shadow > 0) {
        specular = vec3(0.0);
    }

    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * objectColor.rgb;
    FragColor = vec4(lighting, 1.0);
}