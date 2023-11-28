
#ifndef DIFFUSE_LIGHTING_SHADER_H
#define DIFFUSE_LIGHTING_SHADER_H

#include "shaderProgram.h"
#include "vector3D.h"
#include "drawingUtil.h"


const std::string diffuseLightingVertexShader = R"(
	#version 330 core

    layout(location = 0) in vec3 aPos;
    layout(location = 1) in vec3 aNormal;

    // Passes the fragment position to the fragment shader
    out vec3 FragPos;
    // Passes the normal to the fragment shader
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

const std::string diffuseLightingFragmentShader = R"(
	#version 330 core

    // Received from the vertex shader
    in vec3 FragPos;
    // Received from the vertex shader
    in vec3 Normal;

    uniform vec4 objectColor;

    // Maximum number of lights
    #define MAX_LIGHTS 10

    uniform vec3 lightPos[MAX_LIGHTS];
    uniform vec4 lightColors[MAX_LIGHTS];

    // Number of active lights
    uniform int numActiveLights;

    out vec4 FragColor;

    void main(){

        vec3 finalDiffuse = vec3(0.0);

        for (int i = 0; i < numActiveLights; ++i) {
            vec3 lightDir = normalize(lightPos[i] - FragPos);
            float diff =  max(dot(Normal, lightDir), 0.0);
            // Note that in diffuse lighting, the color of the light
            // is irrelevant, only in the phong effect is it considered. 
            vec3 diffuse = objectColor.rgb * diff;
            finalDiffuse += diffuse;
        }

        FragColor = vec4(finalDiffuse, objectColor.a);
    }
)";

namespace pe {

    class DiffuseLightingShader {

    private:

        // Coded into the shader, as it must be known at compile time
        static constexpr int MAXIMUM_NUMBER_OF_LIGHT_SOURCES = 10;

        ShaderProgram shaderProgramObject;

    public:

        DiffuseLightingShader() : shaderProgramObject(
            diffuseLightingVertexShader,
            diffuseLightingFragmentShader) {}

        void drawFaces(
            const std::vector<std::vector<Vector3D>>& faces,
            const glm::mat4& model,
            const glm::mat4& view,
            const glm::mat4& projection,
            const glm::vec4& objectColor,
            int activeLightSources,
            glm::vec3* lightSourcesPosition,
            glm::vec4* lightSourcesColor
        );
    };
}

#endif