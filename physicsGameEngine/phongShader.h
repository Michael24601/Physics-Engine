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
            "phongShader.frag.glsl",
            std::vector<unsigned int>{3, 3, 2}
        ) {}

        void setObjectColor(const glm::vec4& color) {
            setUniform("color", color);
            setUniform("useTexture", false);
        }

        void setObjectTexture(const GLuint& textureId) {
            setTextureUniform(
                "objectTexture",
                textureId,
                GL_TEXTURE_2D,
                0
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

        void setShininess(float shininess) {
            setUniform("shininess", shininess);
        }

    };
}

#endif