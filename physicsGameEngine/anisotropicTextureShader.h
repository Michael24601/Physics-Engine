
#ifndef ANISOTROPIC_TEXTURE_SHADER_H
#define ANISOTROPIC_TEXTURE_SHADER_H

#include "shader.h"

namespace pe {

    class AnisotropicTextureShader : public Shader {

    public:

        AnisotropicTextureShader() : Shader(
            "anisotropicTextureVertexShader.glsl",
            "anisotropicTextureFragmentShader.glsl"
        ) {}

        void setObjectTexture(const GLuint& textureId) {
            setTextureUniform(
                "objectTexture",
                textureId,
                GL_TEXTURE_2D,
                0
            );
        }

        void setModelMatrix(const glm::mat4& model) {
            setUniform("model", model);
        }

        void setViewMatrix(const glm::mat4& view) {
            setUniform("view", view);
        }

        void setProjectionMatrix(const glm::mat4& projection) {
            setUniform("projection", projection);
        }

        void setViewMatrix(const glm::mat4& view) {
            setUniform("view", view);
        }

        void setModelMatrix(const glm::mat4& model) {
            setUniform("model", model);
        }

        void setProjectionMatrix(const glm::mat4& projection) {
            setUniform("projection", projection);
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

        void setViewPosition(const glm::vec3& position) {
            setUniform("viewPos", position);
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