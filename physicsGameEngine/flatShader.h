
#ifndef FLAT_SHADER_H
#define FLAT_SHADER_H

#include "shader.h"
#include "renderComponent.h"

namespace pe {

    class FlatShader : public Shader {

    public:

        FlatShader() : Shader(
            "flatShader.vert.glsl",
            "flatShader.frag.glsl",
            std::vector<unsigned int>{3, 2}
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

        void setObjectData(RenderComponent& renderComponent) override {
            setModelMatrix(renderComponent.model);
            setObjectColor(renderComponent.color);
            setObjectTexture(renderComponent.texture);
        }

    };
}

#endif