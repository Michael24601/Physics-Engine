

#ifndef ADVANCED_DIFFUSE_TEXTURE_SHADER_H
#define ADVANCED_DIFFUSE_TEXTURE_SHADER_H

#include "shader.h"

namespace pe {

    class AdvancedDiffuseTextureShader : public Shader {

    public:

        AdvancedDiffuseTextureShader() : Shader(
            "diffuseTextureShader.vert.glsl",
            "advancedDiffuseTextureShader.frag.glsl"
        ) {}

        void setObjectTexture(const GLuint& textureId) {
            setTextureUniform(
                "objectBaseColorTexture",
                textureId,
                GL_TEXTURE_2D,
                0
            );
        }

        void setAOTexture(const GLuint& textureId) {
            setTextureUniform(
                "objectAOTexture",
                textureId,
                GL_TEXTURE_2D,
                1
            );
        }

        void setRoughnessTexture(const GLuint& textureId) {
            setTextureUniform(
                "objectRoughnessTexture",
                textureId,
                GL_TEXTURE_2D,
                2
            );
        }

        void setDiffuseTexture(const GLuint& textureId) {
            setTextureUniform(
                "objectDiffuseTexture",
                textureId,
                GL_TEXTURE_2D,
                3
            );
        }

        void setNormalTexture(const GLuint& textureId) {
            setTextureUniform(
                "objectNormalTexture",
                textureId,
                GL_TEXTURE_2D,
                4
            );
        }


        void setOpacityTexture(const GLuint& textureId) {
            setTextureUniform(
                "opacityTexture",
                textureId,
                GL_TEXTURE_2D,
                5
            );
        }


        void setLightPosition(const glm::vec3* positions, int size) {
            // Setting an array means sending the first value
            setUniform("lightPos", positions, size);
            setUniform("numActiveLights", size);
        }

    };
}

#endif
