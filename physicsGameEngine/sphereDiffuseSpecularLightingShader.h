/*
    This shader works in the same way as the sphere diffuse lighting
    shader, but with specular lighting (phong).
*/

#ifndef SPHERE_DIFFUSE_SPECULAR_LIGHTING_SHADER_H
#define SPHERE_DIFFUSE_SPECULAR_LIGHTING_SHADER_H

#include "shaderProgram.h"
#include "vector3D.h"
#include "drawingUtil.h"

/*
    The vertex positions are received, but not their normals, because in
    a sphere, normals are always the vector form the center to the vertex,
    so it can be derived from the position.
*/
const std::string sphereDiffuseSpecularLightingVertexShader = R"(
    #version 330 core

    layout(location = 0) in vec3 aPos;

    out vec3 FragPos;

    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    void main() {
        FragPos = vec3(model * vec4(aPos, 1.0));
        gl_Position = projection * view * vec4(FragPos, 1.0);
    }
)";


const std::string sphereDiffuseSpecularLightingFragmentShader = R"(
    #version 330 core

    in vec3 FragPos;

    uniform vec4 objectColor;

    // Maximum number of lights
    #define MAX_LIGHTS 10

    uniform vec3 lightPos[MAX_LIGHTS];
    uniform vec4 lightColors[MAX_LIGHTS];

    // Number of active lights
    uniform int numActiveLights;

    out vec4 FragColor;

    uniform vec3 sphereCenter;
    uniform float sphereRadius;

    // Specular lighting parameters
    uniform vec3 viewPos;  // Camera position
    uniform float shininess;  // Shininess factor

    void main(){
        // Calculate the sphere's normal based on its center and radius
        vec3 normal = normalize(FragPos - sphereCenter);

        vec3 finalDiffuse = vec3(0.0);
        vec3 finalSpecular = vec3(0.0);

        for (int i = 0; i < numActiveLights; ++i) {
            vec3 lightDir = normalize(lightPos[i] - FragPos);
            float diff = max(dot(normal, lightDir), 0.0);
            vec3 diffuse = objectColor.rgb * diff * lightColors[i].rgb;
            finalDiffuse += diffuse;

            // Specular calculation
            vec3 viewDir = normalize(viewPos - FragPos);
            vec3 reflectDir = reflect(-lightDir, normal);
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

    class SphereDiffuseSpecularLightingShader {

    private:

        // Coded into the shader, as it must be known at compile time
        static constexpr int MAXIMUM_NUMBER_OF_LIGHT_SOURCES = 10;

        ShaderProgram shaderProgramObject;

    public:

        SphereDiffuseSpecularLightingShader() : shaderProgramObject(
            sphereDiffuseSpecularLightingVertexShader,
            sphereDiffuseSpecularLightingFragmentShader) {}

        void drawFace(
            const std::vector<std::vector<Vector3D>>& faces,
            const glm::vec3& centerPosition,
            real radius,
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