
#ifndef TEXTURE_SHADER_H
#define TEXTURE_SHADER_H

#include "shader.h"

namespace pe {

    class TextureShader : public Shader {

    public:

        TextureShader() : Shader(
            "textureShader.vert.glsl",
            "textureShader.frag.glsl"
        ) {}

        void setObjectTexture(const GLuint& textureId) {
            setTextureUniform(
                "objectTexture",
                textureId,
                GL_TEXTURE_2D,
                0
            );
        }

    };
}

#endif
