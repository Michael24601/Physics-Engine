/*
    This shader renders a skybox.
    It expects a vertex buffer that contains skybox vertices,
    where the vertices range from (1, 1, 1) to (-1, -1, -1),
    centered around (0, 0, 0) (the texture coordinates of a
    cubemap basically).
    The model matrix can be used to scale the skybox to the scene's
    requiremenets.
*/

#ifndef SKYBOX_SHADER_H
#define SKYBOX_SHADER_H

#include "shader.h"
#include "renderComponent.h"

namespace pe {

    class SkyboxShader : public Shader {

    public:

        SkyboxShader() : Shader(
            std::vector<unsigned int>{3},
            "skyboxShader.vert.glsl",
            "skyboxShader.frag.glsl"
        ) {}

        void setCubemap(GLuint skyboxTextureId) {
            setTextureUniform(
                "cubemap",
                skyboxTextureId,
                GL_TEXTURE_CUBE_MAP,
                0
            );
        }

        void setObjectData(RenderComponent& renderComponent) override {
            setModelMatrix(renderComponent.model);
            setCubemap(renderComponent.cubemapTexture);
        }

    };
}

#endif