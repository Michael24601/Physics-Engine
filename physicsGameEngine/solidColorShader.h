
#ifndef SOLID_COLOR_SHADER_H
#define SOLID_COLOR_SHADER_H

#include "shader.h"
#include "renderComponent.h"

namespace pe {

	class SolidColorShader : public Shader {

	public:

        SolidColorShader() : Shader(
            std::vector<unsigned int>{3},
            "solidColorShader.vert.glsl",
            "solidColorShader.frag.glsl"
        ) {}

        void setObjectColor(const glm::vec4& color) {
            setUniform("color", color);
        }

        void setObjectData(RenderComponent& renderComponent) override {
            setModelMatrix(renderComponent.model);
            setObjectColor(renderComponent.color);
        }

	};
}

#endif