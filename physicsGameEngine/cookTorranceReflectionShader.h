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

        void setEnvironmentMap(GLuint environmentMapTextureId) {
            setTextureUniform(
                "environmentMap",
                environmentMapTextureId, 
                GL_TEXTURE_CUBE_MAP, 
                1
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

        void setReflectionStrength(float reflectionStrength) {
            setUniform("reflectionStrength", reflectionStrength);
        }
    };
}

#endif
