
#ifndef COOK_TORRANCE_REFLECTION_SHADER_WITH_SKYBOX_H
#define COOK_TORRANCE_REFLECTION_SHADER_WITH_SKYBOX_H

#include "shaderProgram.h"
#include "vector3D.h"
#include "drawingUtil.h"
#include "shaderInterface.h"
#include "openglUtility.h"

namespace pe {

    class CookTorranceReflectionShaderWithSkybox {

    private:

        // Coded into the shader, as it must be known at compile time
        static constexpr int MAXIMUM_NUMBER_OF_LIGHT_SOURCES = 10;

        ShaderProgram shaderProgramObject;

    public:

        CookTorranceReflectionShaderWithSkybox() : shaderProgramObject(
            readFileToString("cookTorranceReflectionVertexShader.glsl"),
            readFileToString("cookTorranceReflectionFragmentShaderWithSkybox.glsl")
        ) {}

        /*
            Here, the view position is just the camera position.
            Roughness controls the surface roughness of the material,
            where 0 is very smooth and 1 is very rough.
            Fresnel controls the reflectivity at glancing angles, with
            0 being less reflective, and 1 being more reflective.

            Reflection Stregth is how much the object reflects, and how
            much it is matte, and light influence is how much the light
            influences the reflection (e.g. does the darkness make the
            surface less reflective or not).
        */
        void drawFaces(
            const std::vector<glm::vec3>& faces,
            const std::vector<glm::vec3>& normals,
            const std::vector<glm::vec2>& texCoords,
            const glm::mat4& model,
            const glm::mat4& view,
            const glm::mat4& projection,
            GLuint skybox,
            GLuint environmentMapTextureId,
            const glm::vec4& baseColor,
            int activeLightSources,
            glm::vec3* lightSourcesPosition,
            glm::vec4* lightSourcesColor,
            const glm::vec3& viewPosition,
            real roughness,
            real fresnel,
            real reflectionStrength,
            real lightInfluence
        );
    };
}

#endif
