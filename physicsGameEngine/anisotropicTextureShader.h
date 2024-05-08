
#ifndef ANISOTROPIC_TEXTURE_SHADER_H
#define ANISOTROPIC_TEXTURE_SHADER_H

#include "shader.h"

namespace pe {

    class AnisotropicTextureShader : public Shader {

    public:

        AnisotropicTextureShader() : Shader(
            "anisotropicTextureShader.vert.glsl",
            "anisotropicTextureShader.frag.glsl"
        ) {}

        void setObjectTexture(const GLuint& textureId) {
            setTextureUniform(
                "objectTexture",
                textureId,
                GL_TEXTURE_2D,
                0
            );
        }

        void setSpecularColor(const glm::vec4& color) {
            setUniform("specularColor", color);
        }

        void setAmbientColor(const glm::vec4& color) {
            setUniform("ambientColor", color);
        }

        void setLightPosition(const glm::vec4& position) {
            setUniform("lightPos", position);
        }

        void setLightColor(const glm::vec4& color) {
            setUniform("lightColor", color);
        }

        void setAlphaX(float alphaXValue) {
            setUniform("alphaX", alphaXValue);
        }

        void setAlphaY(float alphaYValue) {
            setUniform("alphaY", alphaYValue);
        }

    };
}

#endif