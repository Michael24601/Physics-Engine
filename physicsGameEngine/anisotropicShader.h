
#ifndef NEW_ANISOTROPIC_SHADER_H
#define NEW_ANISOTROPIC_SHADER_H

#include "shader.h"

namespace pe {

    class AnisotropicShader: public Shader {

    public:

        AnisotropicShader() : Shader(
            "anisotropicVertexShader.glsl",
            "anisotropicFragmentShader.glsl"
        ) {}

        void setModelMatrix(const glm::mat4& model) {
            setUniform("model", model);
        }

        void setViewMatrix(const glm::mat4& view) {
            setUniform("view", view);
        }

        void setProjectionMatrix(const glm::mat4& projection) {
            setUniform("projection", projection);
        }

        void setObjectColor(const glm::vec4& color) {
            setUniform("objectColor", color);
        }

        void setAmbientColor(const glm::vec4& color) {
            setUniform("ambientColor", color);
        }

        void setSpecularColor(const glm::vec4& color) {
            setUniform("specularColor", color);
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
