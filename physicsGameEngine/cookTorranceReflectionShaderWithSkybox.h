
#ifndef COOK_TORRANCE_REFLECTION_SHADER_WITH_SKYBOX_H
#define COOK_TORRANCE_REFLECTION_SHADER_WITH_SKYBOX_H

#include "shader.h"

namespace pe {

    class CookTorranceReflectionShaderWithSkybox : public Shader {

    public:

        CookTorranceReflectionShaderWithSkybox() : Shader(
            "cookTorranceReflectionVertexShader.glsl",
            "cookTorranceReflectionFragmentShaderWithSkybox.glsl"
        ) {}

        void setEnvironmentMap(GLuint environmentMapTextureId) {
            setTextureUniform(
                "environmentMap",
                environmentMapTextureId,
                GL_TEXTURE_CUBE_MAP,
                1
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

        void setRoughness(float roughness) {
            setUniform("roughness", roughness);
        }

        void setFresnel(float fresnel) {
            setUniform("fresnel", fresnel);
        }

        void setActiveLightsCount(int count) {
            setUniform("numActiveLights", count);
        }

        void setReflectionStrength(float reflectionStrength) {
            setUniform("reflectionStrength", reflectionStrength);
        }

        void setLightInfluence(float lightInfluence) {
            setUniform("lightInfluence", lightInfluence);
        }
    };
}

#endif
