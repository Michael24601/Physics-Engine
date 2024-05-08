/*
    Header file for class representing a phong shader.
    A phong shader is one that combines diffuse lighting with specular
    lighting to create a shininess effect.
*/

#ifndef PHONG_SHADER_H
#define PHONG_SHADER_H

#include "shader.h"

namespace pe {

    class PhongShader : public Shader {

    public:

       PhongShader() : Shader(
            "phongShader.vert.glsl",
            "phongShader.frag.glsl"
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

        void setShininess(float shininess) {
            setUniform("shininess", shininess);
        }

    };
}

#endif