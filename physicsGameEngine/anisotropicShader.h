
#ifndef ANISOTROPIC_SHADER_H
#define ANISOTROPIC_SHADER_H

#include "shaderProgram.h"
#include "vector3D.h"
#include "drawingUtil.h"
#include "shaderInterface.h"


const std::string anisotropicVertexShader = R"(
    #version 330 core   

    layout(location = 0) in vec3 aPos;
    layout(location = 1) in vec3 aNormal;
    layout(location = 2) in vec3 aTangent;
    layout(location = 3) in vec3 aBitangent;

    out vec3 FragPos;
    out vec3 Normal;
    out vec3 Tangent;
    out vec3 Bitangent;

    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    void main(){
        FragPos = vec3(model * vec4(aPos, 1.0));
        Normal =vec3(model * vec4(aNormal, 1.0));
        Tangent = vec3(model * vec4(aTangent, 1.0));
        Bitangent = vec3(model * vec4(aBitangent, 1.0));
        gl_Position = projection * view * vec4(FragPos, 1.0);
    }
)";

const std::string anisotropicFragmentShader = R"(
    #version 330 core

    in vec3 FragPos;
    in vec3 Normal;
    in vec3 Tangent;
    in vec3 Bitangent;

    uniform vec4 objectColor;

    #define MAX_LIGHTS 10

    uniform vec3 lightPos[MAX_LIGHTS];
    uniform vec4 lightColors[MAX_LIGHTS];

    uniform int numActiveLights;

    // Anisotropic shading parameters
    uniform vec3 viewPos;  // Camera position
    uniform float roughness;  // Surface roughness
    uniform float fresnel;    // Fresnel factor

    out vec4 FragColor;

    void main(){
        vec3 finalDiffuse = vec3(0.0);
        vec3 finalSpecular = vec3(0.0);

        for (int i = 0; i < numActiveLights; ++i) {
            // Diffuse calculation
            vec3 lightDir = normalize(lightPos[i] - FragPos);
            float diff = max(dot(Normal, lightDir), 0.0);
            vec3 diffuse = objectColor.rgb * diff;
            finalDiffuse += diffuse;

            // Anisotropic specular calculation
            vec3 viewDir = normalize(viewPos - FragPos);
            vec3 halfwayDir = normalize(lightDir + viewDir);
            vec3 reflectDir = reflect(-lightDir, Normal);
            vec3 h = normalize(viewDir + lightDir);
            float NdotH = max(dot(Normal, halfwayDir), 0.0);
            float D = (roughness * roughness) / (3.14159265359 * pow((dot(h, Tangent) * dot(h, Bitangent) * (roughness * roughness - 1.0) + 1.0), 2.0));
            float G = min(1.0, min(2.0 * NdotH * dot(Normal, viewDir) / dot(viewDir, halfwayDir), 2.0 * NdotH * dot(Normal, lightDir) / dot(lightDir, halfwayDir)));
            float F = fresnel + (1.0 - fresnel) * pow(1.0 - NdotH, 5.0);

            // The opacity of the anisotropic shading effect is determined through the alpha value
            vec3 specular = lightColors[i].rgb * (D * G * F) / (4.0 * dot(Normal, viewDir) * dot(Normal, lightDir));
            finalSpecular += specular;
        }

        // Combines diffuse and specular with some ambient lighting (0.1 looks best)
        // Adjusts the ambient factor as needed
        vec3 ambientColor = 0.1 * objectColor.rgb;
        vec3 resultColor = finalDiffuse + finalSpecular + ambientColor;

        FragColor = vec4(resultColor, objectColor.a);
    }
)";

namespace pe {

    class AnisotropicShader {

    private:

        // Coded into the shader, as it must be known at compile time
        static constexpr int MAXIMUM_NUMBER_OF_LIGHT_SOURCES = 10;

        ShaderProgram shaderProgramObject;

    public:

        AnisotropicShader() : shaderProgramObject(
            anisotropicVertexShader,
            anisotropicFragmentShader
        ) {}

        /*
            Here, the view position is just the camera position.
            Roughness controls the surface roughness of the material,
            where 0 is very smooth and 1 is very rough.
            Fresnel controls the reflectivity at glancing angles, with
            0 being less reflective, and 1 being more reflective.
        */
        void drawFaces(
            const std::vector<glm::vec3>& faces,
            const std::vector<glm::vec3>& normals,
            const std::vector<glm::vec3>& tangents,
            const std::vector<glm::vec3>& bitangents,
            const glm::mat4& model,
            const glm::mat4& view,
            const glm::mat4& projection,
            const glm::vec4& objectColor,
            int activeLightSources,
            glm::vec3* lightSourcesPosition,
            glm::vec4* lightSourcesColor,
            const glm::vec3& viewPosition,
            float roughness,
            float fresnel
        );
    };
}

#endif
