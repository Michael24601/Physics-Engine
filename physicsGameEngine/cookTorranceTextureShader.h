

#ifndef COOK_TORRANCE_TEXTURE_SHADER_H
#define COOK_TORRANCE_TEXTURE_SHADER_H

#include "shader.h"

namespace pe {

    class CookTorranceTextureShader : public Shader {

    public:

        CookTorranceTextureShader() : Shader(
            "cookTorranceTextureShader.vert.glsl",
            "cookTorranceTextureShader.frag.glsl"
        ) {}

        void setObjectTexture(const GLuint& textureId) {
            setTextureUniform(
                "objectTexture",
                textureId,
                GL_TEXTURE_2D,
                0
            );
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
