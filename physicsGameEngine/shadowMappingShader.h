

#ifndef SHADOW_MAPPING_SHADER_H
#define SHADOW_MAPPING_SHADER_H

#include "shader.h"

namespace pe {

    class ShadowMappingShader : public Shader {

    public:

        ShadowMappingShader() : Shader (
            "shadowMappingVertexShader.glsl",
            "shadowMappingFragmentShader.glsl"
        ) {}

        void setShadowMap(const GLuint& textureId) {
            setTextureUniform(
                "shadowMap",
                textureId,
                GL_TEXTURE_2D,
                0
            );
        }

        void setObjectColor(const glm::vec4& color) {
            setUniform("objectColor", color);
        }

        void setLightPosition(const glm::vec3& position) {
            // Setting an array means sending the first value
            setUniform("lightPos", position);
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
