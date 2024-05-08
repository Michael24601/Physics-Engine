
#ifndef DIFFUSE_LIGHTING_SHADER_H
#define DIFFUSE_LIGHTING_SHADER_H

#include "shader.h"

namespace pe {

    class DiffuseLightingShader : public Shader{

    public:

        DiffuseLightingShader() : Shader(
            "diffuseLightingShader.vert.glsl",
            "diffuseLightingShader.frag.glsl"
        ) {}

        void setObjectColor(const glm::vec4& color) {
            setUniform("objectColor", color);
        }

        void setLightPosition(const glm::vec3* positions, int size) {
            // Setting an array means sending the first value
            setUniform("lightPos", positions, size);
            setUniform("numActiveLights", size);
        }

    };
}

#endif