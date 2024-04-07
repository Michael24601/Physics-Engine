
#ifndef NEW_ANISOTROPIC_SHADER_H
#define NEW_ANISOTROPIC_SHADER_H

#include "shaderProgram.h"
#include "vector3D.h"
#include "drawingUtil.h"
#include "shaderInterface.h"
#include "openglUtility.h"

namespace pe {

    class NewAnisotropicShader {

    private:

        // Coded into the shader, as it must be known at compile time
        static constexpr int MAXIMUM_NUMBER_OF_LIGHT_SOURCES = 10;

        ShaderProgram shaderProgramObject;

    public:

        NewAnisotropicShader() : shaderProgramObject(
            readFileToString("anisotropicVertexShader.glsl"),
            readFileToString("newAnisotropicFragmentShader.glsl")
        ) {}


        void drawFaces(
            std::vector<glm::vec3>& faces,
            std::vector<glm::vec3>& normals,
            std::vector<glm::vec3>& tangents,
            std::vector<glm::vec3>& bitangents,
            const glm::mat4& model,
            const glm::mat4& view,
            const glm::mat4& projection,
            const glm::vec4& objectColor,
            const glm::vec4& specularColor,
            const glm::vec4& ambientColor,
            const glm::vec3& lightSourcePosition,
            const glm::vec4& lightSourceColor,
            const glm::vec3& viewPosition,
            real alphaX,
            real alphaY
        );
    };
}

#endif
