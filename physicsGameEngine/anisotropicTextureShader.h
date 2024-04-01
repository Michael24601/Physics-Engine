
#ifndef ANISOTROPIC_TEXTURE_SHADER_H
#define ANISOTROPIC_TEXTURE_SHADER_H

#include "shaderProgram.h"
#include "vector3D.h"
#include "drawingUtil.h"
#include "shaderInterface.h"
#include "openglUtility.h"

namespace pe {

    class AnisotropicTextureShader {

    private:

        // Coded into the shader, as it must be known at compile time
        static constexpr int MAXIMUM_NUMBER_OF_LIGHT_SOURCES = 10;

        ShaderProgram shaderProgramObject;

    public:

        AnisotropicTextureShader() : shaderProgramObject(
            readFileToString("anisotropicTextureVertexShader.glsl"),
            readFileToString("anisotropicTextureFragmentShader.glsl")
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
            const std::vector<glm::vec2>& texCoords,
            const glm::mat4& model,
            const glm::mat4& view,
            const glm::mat4& projection,
            GLuint textureID,
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