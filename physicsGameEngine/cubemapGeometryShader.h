/*
    This is the header file for a class representing a cubemap shader
    used for rendering the depth map of a scene. Unlike the simple
    shader however, it doesn't just output the depth value of the shape
    being rendered.
    Since we need a cubemap of a shadow, we technically need to render
    the scene 6 times, one for each cube face. However, since that is
    too slow, we resolve to instead use a geometry shader that can
    generate 6 triangles, one for each side, only needing to render
    the scene once.
*/

#ifndef CUBEMAP_GEOMETRY_SHADER_H
#define CUBEMAP_GEOMETRY_SHADER_H

#include "shader.h"
#include "renderComponent.h"

namespace pe {

    class CubemapGeometryShader : public Shader {

    public:

        CubemapGeometryShader() : Shader(
            std::vector<unsigned int>{3},
            "cubemapShaderGeometry.vert.glsl",
            "cubemapShader.frag.glsl",
            "cubemapShaderGeometry.geom.glsl"
        ) {}

        void setFarPlane(float farPlane) {
            setUniform("farPlane", farPlane);
        }

        void setViewMatrices(const std::vector<glm::mat4> viewMatrices) {
            if (viewMatrices.size() != 6) {
                throw std::invalid_argument("The cubemap shader takes 6 view matrices");
            }
            setUniform("viewMatrices", viewMatrices.data(), 6);
        }

        void setLightPosition(const glm::vec3& position) {
            setUniform("lightPos", position);
        }

        void setObjectData(RenderComponent& renderComponent) override {
            setModelMatrix(renderComponent.model);
        }

    };
}

#endif