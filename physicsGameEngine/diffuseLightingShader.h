
#ifndef DIFFUSE_LIGHTING_SHADER_H
#define DIFFUSE_LIGHTING_SHADER_H

#include "shader.h"

namespace pe {

    class DiffuseLightingShader : public Shader{

    public:

        DiffuseLightingShader() : Shader(
            "diffuseLightingVertexShader.glsl",
            "diffuseLightingFragmentShader.glsl"
        ) {}

        void setObjectColor(const glm::vec4& color) {
            setUniform("objectColor", color);
        }

        void setLightPosition(const glm::vec3* positions) {
            // Setting an array means sending the first value
            setUniform("lightPos", positions[0]);
        }

        void setActiveLightsCount(int count) {
            setUniform("numActiveLights", count);
        }
    };
}

#endif