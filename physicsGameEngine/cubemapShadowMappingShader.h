

#ifndef CUBEMAP_SHADOW_MAPPING_SHADER_H
#define CUBEMAP_SHADOW_MAPPING_SHADER_H

#include "shader.h"
#include "renderComponent.h"

namespace pe {

    class CubemapShadowMappingShader : public Shader {

    public:

        CubemapShadowMappingShader() : Shader(
            std::vector<unsigned int>{3, 3, 2},
            "cubemapShadowMappingShader.vert.glsl",
            "cubemapShadowMappingShader.frag.glsl"
        ) {}


        void setCubemapShadowMap(const GLuint& textureId) {
            setTextureUniform(
                "shadowCubemap",
                textureId,
                GL_TEXTURE_CUBE_MAP,
                1
            );
        }


        void setObjectColor(const glm::vec4& color) {
            setUniform("color", color);
            setUniform("useTexture", false);
        }

        void setObjectTexture(const GLuint& textureId) {

            if (textureId == 0) return;

            setTextureUniform(
                "objectTexture",
                textureId,
                GL_TEXTURE_2D,
                0
            );
            setUniform("useTexture", true);
        }

        void setLightPosition(const glm::vec3& position) {
            // Setting an array means sending the first value
            setUniform("lightPos", position);
        }

        void setFarPlane(float farPlane) {
            setUniform("farPlane", farPlane);
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

        void setObjectData(RenderComponent& renderComponent) override {
            setModelMatrix(renderComponent.model);
            setObjectColor(renderComponent.color);
            setObjectTexture(renderComponent.texture);
        }

    };
}

#endif
