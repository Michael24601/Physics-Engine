/*
    This is the header file for a class representing a cubemap shader
    used for rendering the depth map of a scene. Unlike the simple
    shader however, it doesn't just output the depth value of the shape
    being rendered.
    Since we need a cubemap of a shadow, we technically need to render
    the scence 6 times, one for each cube face. However, since that is
    too slow, we resolve to instead use a geometry shader that can
    generate 6 triangles, one for each side, only needing to render
    the scene once.
    If we want to stop doing that, then what we do is, we remove the
    geometry shader, we edit the vertex shader such that it actually
    applies the view and projection transformations to gl_Position
    (since the geometry shader won't do it), and we edit the
    depth mapper to render the scene 6 times from 6 different views.
    That is always worse however.
*/

#ifndef CUBEMAP_SHADER_H
#define CUBEMAP_SHADER_H

#include "shader.h"
#include "renderComponent.h"

namespace pe {

    class CubemapShader : public Shader {

    public:

        CubemapShader() : Shader(
            std::vector<unsigned int>{3},
            "cubemapShader.vert.glsl",
            "cubemapShader.frag.glsl",
            "cubemapShader.geom.glsl"
        ) {}

        void setFarPlane(float farPlane) {
            setUniform("farPlane", farPlane);
        }

        void setLightPosition(const glm::vec3& position) {
            setUniform("lightPos", position);
        }

        void setViewMatrices(const std::vector<glm::mat4> viewMatrices) {
            if (viewMatrices.size() != 6) {
                throw std::invalid_argument("The cubemap shader takes 6 view matrices");
            }
            setUniform("viewMatrices", viewMatrices.data(), 6);
        }

        void setObjectData(RenderComponent& renderComponent) override {
            setModelMatrix(renderComponent.model);
        }

    };
}

#endif