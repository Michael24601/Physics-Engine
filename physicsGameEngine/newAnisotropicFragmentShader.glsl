
#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec3 Tangent;
in vec3 Bitangent;

uniform vec4 objectColor;
uniform vec4 ambientColor;
uniform vec4 specularColor;

uniform vec4 lightPos;
uniform vec4 lightColor;

// Anisotropic shading parameters
uniform vec3 viewPos;
uniform float alphaX;
uniform float alphaY;

out vec4 FragColor;

void main() {

    vec3 normalDirection = normalize(Normal);
    vec3 tangentDirection = normalize(Tangent);

    vec3 viewDirection = normalize(viewPos - FragPos);
    vec3 lightDirection;
    float attenuation;

    // directional light?
    if (0.0 == lightPos.w) {
        attenuation = 1.0; // no attenuation
        lightDirection = normalize(vec3(lightPos));
    } 
    // point or spot light
    else {
        vec3 vertexToLightSource = vec3(lightPos) - vec3(FragPos);
        float distance = length(vertexToLightSource);
        attenuation = 1.0 / distance; // linear attenuation 
        lightDirection = normalize(vertexToLightSource);
    }
            
    vec3 halfwayVector = 
        normalize(lightDirection + viewDirection);
    vec3 binormalDirection = 
        cross(normalDirection, tangentDirection);
    float dotLN = dot(lightDirection, normalDirection); 
        // compute this dot product only once
            
    vec3 ambientLighting = vec3(ambientColor) 
        * vec3(objectColor);

    vec3 diffuseReflection = attenuation * vec3(lightColor) 
        * vec3(objectColor) * max(0.0, dotLN);
            
    vec3 specularReflection;
    if (dotLN < 0.0) // light source on the wrong side?
    {
        specularReflection = vec3(0.0, 0.0, 0.0); 
            // no specular reflection
    }
    else // light source on the right side
    {
        float dotHN = dot(halfwayVector, normalDirection);
        float dotVN = dot(viewDirection, normalDirection);
        float dotHTAlphaX = 
            dot(halfwayVector, tangentDirection) / alphaX;
        float dotHBAlphaY = dot(halfwayVector, 
            binormalDirection) / alphaY;

        specularReflection = attenuation * vec3(specularColor) 
            * sqrt(max(0.0, dotLN / dotVN)) 
            * exp(-2.0 * (dotHTAlphaX * dotHTAlphaX 
            + dotHBAlphaY * dotHBAlphaY) / (1.0 + dotHN));
    }

    FragColor = vec4(ambientLighting 
        + diffuseReflection + specularReflection, 1.0);
}