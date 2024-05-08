
#ifndef COOK_TORRANCE_REFLECTION_SHADER_H
#define COOK_TORRANCE_REFLECTION_SHADER_H

#include "shader.h"

namespace pe {

    class CookTorranceReflectionShader : public Shader {

    public:

        CookTorranceReflectionShader() : Shader(
            "cookTorranceReflectionShader.vert.glsl",
            "cookTorranceReflectionShader.frag.glsl"
        ) {}

        void setEnvironmentMap(GLuint environmentMapTextureId) {
            setTextureUniform(
                "environmentMap",
                environmentMapTextureId, 
                GL_TEXTURE_CUBE_MAP, 
                1
            );
        }

        void setLightPosition(const glm::vec3* positions, int size) {
            // Setting an array means sending the first value
            setUniform("lightPos", positions, size);
            setUniform("numActiveLights", size);
        }

        void setLightColors(const glm::vec4* colors, int size) {
            setUniform("lightColors", colors, size);
        }

        void setBaseColor(const glm::vec4& baseColor) {
            setUniform("objectColor", baseColor);
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
