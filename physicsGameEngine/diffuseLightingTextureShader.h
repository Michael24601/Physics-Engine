
#ifndef DIFFUSE_LIGHTING_TEXTURE_SHADER_H
#define DIFFUSE_LIGHTING_TEXTURE_SHADER_H

#include "shaderProgram.h"
#include "vector3D.h"
#include "drawingUtil.h"
#include "shaderInterface.h"
#include "openglUtility.h"

namespace pe {

    class DiffuseLightingTextureShader {

    private:

        // Coded into the shader, as it must be known at compile time
        static constexpr int MAXIMUM_NUMBER_OF_LIGHT_SOURCES = 10;

        ShaderProgram shaderProgramObject;

    public:

        DiffuseLightingTextureShader() : shaderProgramObject(
            readFileToString("diffuseLightingTextureVertexShader.glsl"),
            readFileToString("diffuseLightingTextureFragmentShader.glsl")
        ) {}

        void drawFaces(
            const std::vector<glm::vec3>& faces,
            const std::vector<glm::vec3>& normals,
            const std::vector<glm::vec2>& texCoords,
            const glm::mat4& model,
            const glm::mat4& view,
            const glm::mat4& projection,
            GLuint textureID, // Texture ID
            int activeLightSources,
            glm::vec3* lightSourcesPosition,
            glm::vec4* lightSourcesColor
        );
    };
}

#endif
