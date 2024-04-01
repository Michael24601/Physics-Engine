
#ifndef DIFFUSE_SPECULAR_LIGHTING_SHADER_H
#define DIFFUSE_SPECULAR_LIGHTING_SHADER_H

#include "shaderProgram.h"
#include "vector3D.h"
#include "drawingUtil.h"
#include "shaderInterface.h"
#include "openglUtility.h"

namespace pe {

    class DiffuseSpecularLightingShader {

    private:

        // Coded into the shader, as it must be known at compile time
        static constexpr int MAXIMUM_NUMBER_OF_LIGHT_SOURCES = 10;

        ShaderProgram shaderProgramObject;

    public:

        DiffuseSpecularLightingShader() : shaderProgramObject(
            readFileToString("phongVertexShader.glsl"),
            readFileToString("phongFragmentShader.glsl")
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