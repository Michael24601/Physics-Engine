
#ifndef DIFFUSE_TEXTURE_SHADER_H
#define DIFFUSE_TEXTURE_SHADER_H

#include "shader.h"

namespace pe {

    class DiffuseTextureShader : public Shader{

    public:

        DiffuseTextureShader() : Shader(
            "diffuseTextureShader.vert.glsl",
            "diffuseTextureShader.frag.glsl"
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

    };
}

#endif
