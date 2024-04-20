
#ifndef COOK_TORRANCE_SHADER_H
#define COOK_TORRANCE_SHADER_H

#include "shader.h"

namespace pe {

    class CookTorranceShader : public Shader {

    public:

        CookTorranceShader() : Shader(
            "cookTorranceVertexShader.glsl",
            "cookTorranceFragmentShader.glsl"
        ) {}

        void setObjectColor(const glm::vec4& color) {
            setUniform("objectColor", color);
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

        void setRoughness(float roughness) {
            setUniform("roughness", roughness);
        }

        void setFresnel(float fresnel) {
            setUniform("fresnel", fresnel);
        }
    };
}

#endif
