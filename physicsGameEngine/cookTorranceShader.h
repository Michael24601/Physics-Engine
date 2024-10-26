
#ifndef COOK_TORRANCE_SHADER_H
#define COOK_TORRANCE_SHADER_H

#include "shader.h"

namespace pe {

    class CookTorranceShader : public Shader {

    public:

        CookTorranceShader() : Shader(
            "cookTorranceShader.vert.glsl",
            "cookTorranceShader.frag.glsl",
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

        void setRoughness(float roughness) {
            setUniform("roughness", roughness);
        }

        void setFresnel(float fresnel) {
            setUniform("fresnel", fresnel);
        }
    };
}

#endif
