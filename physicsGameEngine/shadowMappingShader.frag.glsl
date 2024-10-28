
#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
in vec4 FragPosLightSpace;
in vec3 ViewPos;

uniform vec4 color;
uniform sampler2D objectTexture;
uniform bool useTexture;

uniform vec3 lightPos;

uniform bool PCF;

uniform float shadowStrength;

uniform sampler2D shadowMap;

out vec4 FragColor;

float ShadowCalculation(vec4 fragPosLightSpace, vec3 lightDir){
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective 
    // (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 

    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    // The bias is used so that the shadow of an object is offset to the
    // back by a small value. This is done to ensure that the object
    // does not cast a shadow on itself (since its shadow will be 
    // immediatly on top of it, and small numericall inaccuracies
    // can cause it to overshadow the shape, depending on the angle).
    float bias = max(0.05 * (1.0 - dot(Normal, lightDir)), 0.05);  

    // check whether current frag pos is in shadow
    float shadow = currentDepth - bias > closestDepth ? shadowStrength : 0.0;

    return shadow;
}


float PCFShadowCalculation(vec4 fragPosLightSpace, vec3 lightDir){
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    float currentDepth = projCoords.z;

    // The bias is used so that the shadow of an object is offset to the
    // back by a small value. This is done to ensure that the object
    // does not cast a shadow on itself (since its shadow will be 
    // immediatly on top of it, and small numericall inaccuracies
    // can cause it to overshadow the shape, depending on the angle).
    float bias = max(0.05 * (1.0 - dot(Normal, lightDir)), 0.05);  
    
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth ?  shadowStrength : 0.0;        
        }    
    }
    shadow /= 9.0;

    return shadow;
}


void main(){

    // When we disable face culling in order to draw both sides
    // of the faces of an object (such as when drawing cloth),
    // not only is the front face drawn, but also the back face
    // (which is the same face but with the vertices in reverse
    // order). The noraml of said face is the same as the front
    // face but inverted.
    vec3 normal = gl_FrontFacing ? Normal : -Normal;

    vec4 objectColor;
    if(useTexture)
        objectColor = texture(objectTexture, TexCoord);
    else
        objectColor = color;

    vec3 lightColor = vec3(1.0);
    // ambient
    vec3 ambient = 0.15 * lightColor;
    // diffuse
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // specular
    vec3 viewDir = normalize(ViewPos - FragPos);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;  
    
    // The shadow component
    float shadow = (PCF ? 
        PCFShadowCalculation(FragPosLightSpace, lightDir) :
        ShadowCalculation(FragPosLightSpace, lightDir)
    );

    // If there is a shadow, there shouldn't be anny shine
    if(shadow > 0){
        specular = vec3(0.0);
    }

    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) 
        * objectColor.rgb; 

    FragColor = vec4(lighting, 1.0);
}

