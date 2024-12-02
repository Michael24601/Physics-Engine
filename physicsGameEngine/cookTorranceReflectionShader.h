
#ifndef COOK_TORRANCE_REFLECTION_SHADER_H
#define COOK_TORRANCE_REFLECTION_SHADER_H

#include "shader.h"
#include "renderComponent.h"

namespace pe {

    class CookTorranceReflectionShader : public Shader {

    public:

        CookTorranceReflectionShader() : Shader(
            std::vector<unsigned int>{3, 3, 2},
            "cookTorranceReflectionShader.vert.glsl",
            "cookTorranceReflectionShader.frag.glsl"
        ) {}

        void setEnvironmentMap(
            GLuint environmentMapTextureId, 
            int activeTexture = 2
        ) {
            setTextureUniform(
                "environmentMap",
                environmentMapTextureId, 
                GL_TEXTURE_CUBE_MAP, 
                activeTexture
            );
        }

        void setSkybox(GLuint skyboxTextureId) {
            setTextureUniform(
                "skybox",
                skyboxTextureId,
                GL_TEXTURE_CUBE_MAP,
                0
            );
        }

        void setObjectColor(const glm::vec4& color) {
            setUniform("color", color);
            setUniform("useTexture", false);
        }

        void setObjectTexture(const GLuint& textureId) {

            if (textureId == 0) return;

            setTextureUniform(
                "objectTexture",
                textureId,
                GL_TEXTURE_2D,
                1
            );
            setUniform("useTexture", true);
        }

        void setLightPosition(const glm::vec3* positions, int size) {
            // Setting an array means sending the first value
            setUniform("lightPos", positions, size);
            setUniform("numActiveLights", size);
        }

        void setLightColors(const glm::vec4* colors, int size) {
            setUniform("lightColors", colors, size);
        }

        void setFresnel(float fresnel) {
            setUniform("fresnel", fresnel);
        }

        void setRoughness(float roughness) {
            setUniform("roughness", roughness);
        }

        void setReflectionStrength(float reflectionStrength) {
            setUniform("reflectionStrength", reflectionStrength);
        }

        void setLightInfluence(float lightInfluence) {
            setUniform("lightInfluence", lightInfluence);
        }

        void setObjectData(RenderComponent& renderComponent) override {
            setModelMatrix(renderComponent.model);
            setObjectColor(renderComponent.color);
            setObjectTexture(renderComponent.texture);
            setEnvironmentMap(renderComponent.environmentMap);
        }
    };
}

#endif
