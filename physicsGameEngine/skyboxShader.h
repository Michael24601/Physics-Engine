
#ifndef SKYBOX_SHADER_H
#define SKYBOX_SHADER_H

#include "shaderProgram.h"
#include "vector3D.h"
#include "drawingUtil.h"
#include "shaderInterface.h"
#include "openglUtility.h"

namespace pe {

    class SkyboxShader {
    private:

        ShaderProgram shaderProgramObject;

    public:

        SkyboxShader() : shaderProgramObject(
            readFileToString("skyboxShader.glsl"),
            readFileToString("skyboxShader.glsl")
        ) {}

        void drawSkybox(
            const glm::mat4& model,
            const glm::mat4& view,
            const GLuint& cubemap
        );
    };
}

#endif