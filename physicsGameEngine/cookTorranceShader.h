
#ifndef COOK_TORRANCE_SHADER_H
#define COOK_TORRANCE_SHADER_H

#include "shader.h"

namespace pe {

    class CookTorranceShader : public Shader {

    public:

        CookTorranceShader() : Shader(
            "cookTorranceShader.vert.glsl",
            "cookTorranceShader.frag.glsl"
        ) {}

        void setObjectColor(const glm::vec4& color) {
            setUniform("objectColor", color);
        }

        void setLightPosition(const glm::vec3* positions, int size) {
            // Setting an array means sending the first value
            setUniform("lightPos", positions, size);
            setUniform("numActiveLights", size);
        }

        void setLightColors(const glm::vec4* colors, int size) {
            setUniform("lightColors", colors, size);
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
