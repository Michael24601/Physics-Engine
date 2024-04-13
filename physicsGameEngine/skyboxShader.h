
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
            readFileToString("skyboxVertexShader.glsl"),
            readFileToString("skyboxFragmentShader.glsl")
        ) {}

        /*
            The cubemap is a 1 by 1 by 1 cube with its 6 internal faces
            rendered to simulate a 3D background using a cubemap. The size
            parameter scales this cube to fit the whole scene.
        */
        void drawSkybox(
            const glm::mat4& view,
            const glm::mat4& projection,
            const GLuint& cubemap,
            real size
        );
    };
}

#endif