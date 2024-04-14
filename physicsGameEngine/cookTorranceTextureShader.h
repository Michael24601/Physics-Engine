

#ifndef COOK_TORRANCE_TEXTURE_SHADER_H
#define COOK_TORRANCE_TEXTURE_SHADER_H

#include "shader.h"

namespace pe {

    class CookTorranceTextureShader : public Shader {

    public:

        CookTorranceTextureShader() : Shader(
            "cookTorranceTextureVertexShader.glsl",
            "cookTorranceTextureFragmentShader.glsl"
        ) {}

        void setObjectTexture(const GLuint& textureId) {
            setTextureUniform(
                "objectTexture",
                textureId,
                GL_TEXTURE_2D,
                0
            );
        }

        void setModelMatrix(const glm::mat4& model) {
            setUniform("model", model);
        }

        void setViewMatrix(const glm::mat4& view) {
            setUniform("view", view);
        }

        void setProjectionMatrix(const glm::mat4& projection) {
            setUniform("projection", projection);
        }

        void setLightPosition(const glm::vec3* positions) {
            // Setting an array means sending the first value
            setUniform("lightPos", positions[0]);
        }

        void setLightColors(const glm::vec4* colors) {
            setUniform("lightColors", colors[0]);
        }

        void setActiveLightsCount(int count) {
            setUniform("numActiveLights", count);
        }

        void setViewPosition(const glm::vec3& viewPos) {
            setUniform("viewPos", viewPos);
        }

        void setRoughness(float roughness) {
            setUniform("roughness", roughness);
        }

        void setFresnel(float fresnel) {
            setUniform("fresnel", fresnel);
        }
    };
}

#endif
