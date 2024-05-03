
#ifndef SHADOW_MAPPING_TEXTURE_SHADER_H
#define SHADOW_MAPPING_TEXTURE_SHADER_H

#include "shader.h"

namespace pe {

    class ShadowMappingTextureShader : public Shader {

    public:

        ShadowMappingTextureShader() : Shader(
            "shadowMappingTextureVertexShader.glsl",
            "shadowMappingTextureFragmentShader.glsl"
        ) {}

        void setShadowMap(const GLuint& textureId) {
            setTextureUniform(
                "shadowMap",
                textureId,
                GL_TEXTURE_2D,
                1
            );
        }

        void setObjectTexture(const GLuint& textureId) {
            setTextureUniform(
                "objectTexture",
                textureId,
                GL_TEXTURE_2D,
                0
            );
        }

        void setLightPosition(const glm::vec3& position) {
            // Setting an array means sending the first value
            setUniform("lightPos", position);
        }

        void setShadowStrength(float strength) {
            setUniform("shadowStrength", strength);
        }

        /*
            Percentage-closer filtering, or PCF, is used to lessen the
            jaggedness of the edges of shadows.
        */
        void setPCF(bool PCF) {
            setUniform("PCF", PCF);
        }

        /*
            The lightSpace matrix is the projection * view matrix used
            in rendering the shadow map from the light's perspective.
        */
        void setLightSpaceMatrix(const glm::mat4& lightSpace) {
            setUniform("lightSpace", lightSpace);
        }
    };
}

#endif
