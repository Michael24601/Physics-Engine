#ifndef COOK_TORRANCE_REFLECTION_SHADER_H
#define COOK_TORRANCE_REFLECTION_SHADER_H

#include "shader.h"

namespace pe {

    class CookTorranceReflectionShader : public Shader {

    public:

        CookTorranceReflectionShader() : Shader(
            "cookTorranceReflectionVertexShader.glsl",
            "cookTorranceReflectionFragmentShader.glsl"
        ) {}

        void setModelMatrix(const glm::mat4& model) {
            setUniform("model", model);
        }

        void setViewMatrix(const glm::mat4& view) {
            setUniform("view", view);
        }

        void setProjectionMatrix(const glm::mat4& projection) {
            setUniform("projection", projection);
        }

        void setEnvironmentMap(GLuint environmentMapTextureId) {
            setTextureUniform(
                "environmentMap",
                environmentMapTextureId, 
                GL_TEXTURE_CUBE_MAP, 
                1
            );
        }

        void setViewMatrix(const glm::mat4& viewMatrix) {
            setUniform("view", viewMatrix);
        }

        void setModelMatrix(const glm::mat4& modelMatrix) {
            setUniform("model", modelMatrix);
        }

        void setProjectionMatrix(const glm::mat4& projectionMatrix) {
            setUniform("projection", projectionMatrix);
        }

        void setLightPosition(const glm::vec3* positions) {
            setUniform("lightPos", positions[0]);
        }

        void setBaseColor(const glm::vec4& baseColor) {
            setUniform("objectColor", baseColor);
        }

        void setLightColors(const glm::vec4* colors) {
            setUniform("lightColors", colors[0]);
        }

        void setNumActiveLights(int numActiveLights) {
            setUniform("numActiveLights", numActiveLights);
        }

        void setViewPosition(const glm::vec3& viewPosition) {
            setUniform("viewPos", viewPosition);
        }

        void setRoughness(float roughness) {
            setUniform("roughness", roughness);
        }

        void setFresnel(float fresnel) {
            setUniform("fresnel", fresnel);
        }

        void setReflectionStrength(float reflectionStrength) {
            setUniform("reflectionStrength", reflectionStrength);
        }
    };
}

#endif
