
#ifndef DIFFUSE_SPECULAR_LIGHTING_SHADER_H
#define DIFFUSE_SPECULAR_LIGHTING_SHADER_H

#include "shaderProgram.h"
#include "vector3D.h"
#include "drawingUtil.h"
#include "shaderInterface.h"

const std::string diffuseSpecularLightingVertexShader = R"(
    #version 330 core

    layout(location = 0) in vec3 aPos;
    layout(location = 1) in vec3 aNormal;

    out vec3 FragPos;
    out vec3 Normal;

    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    void main(){
        FragPos = vec3(model * vec4(aPos, 1.0));
        Normal = mat3(transpose(inverse(model))) * aNormal;
        gl_Position = projection * view * vec4(FragPos, 1.0);
    }
)";

const std::string diffuseSpecularLightingFragmentShader = R"(
    #version 330 core

    in vec3 FragPos;
    in vec3 Normal;

    uniform vec4 objectColor;

    #define MAX_LIGHTS 10

    uniform vec3 lightPos[MAX_LIGHTS];
    uniform vec4 lightColors[MAX_LIGHTS];

    uniform int numActiveLights;

    // Specular lighting parameters
    uniform vec3 viewPos;  // Camera position
    uniform float shininess;  // Shininess factor

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

            // Specular calculation
            vec3 viewDir = normalize(viewPos - FragPos);
            vec3 reflectDir = reflect(-lightDir, Normal);
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
            // The opacity of the phong effect is determined through the alpha value
            vec3 specular = lightColors[i].rgb * spec * lightColors[i].a;
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

    class DiffuseSpecularLightingShader {

    private:

        // Coded into the shader, as it must be known at compile time
        static constexpr int MAXIMUM_NUMBER_OF_LIGHT_SOURCES = 10;

        ShaderProgram shaderProgramObject;

    public:

        DiffuseSpecularLightingShader() : shaderProgramObject(
            diffuseSpecularLightingVertexShader,
            diffuseSpecularLightingFragmentShader
        ) {}

        /*
            Here, the view position is just the camera position.
            Shininess is how shiny the specular effect is, with
            1 being extremly strong, and 100000 being very weak.
        */
        void drawFaces(
            const std::vector<glm::vec3>& faces,
            const std::vector<glm::vec3>& normals,
            const glm::mat4& model,
            const glm::mat4& view,
            const glm::mat4& projection,
            const glm::vec4& objectColor,
            int activeLightSources,
            glm::vec3* lightSourcesPosition,
            glm::vec4* lightSourcesColor,
            const glm::vec3& viewPosition,
            real shininess
        );
    };
}

#endif