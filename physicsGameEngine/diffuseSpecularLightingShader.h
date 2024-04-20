
#ifndef DIFFUSE_SPECULAR_LIGHTING_SHADER_H
#define DIFFUSE_SPECULAR_LIGHTING_SHADER_H

#include "shader.h"

namespace pe {

    class DiffuseSpecularLightingShader : public Shader {

    public:

        DiffuseSpecularLightingShader() : Shader(
            "phongVertexShader.glsl",
            "phongFragmentShader.glsl"
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

        void setShininess(float shininess) {
            setUniform("shininess", shininess);
        }

    };
}

#endif