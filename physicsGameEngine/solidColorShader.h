
#ifndef SOLID_COLOR_SHADER_H
#define SOLID_COLOR_SHADER_H

#include "shader.h"

namespace pe {

	class SolidColorShader : public Shader {

	public:

        SolidColorShader() : Shader(
            "solidColorVertexShader.glsl",
            "solidColorFragmentShader.glsl"
        ) {}

        void setModelMatrix(const glm::mat4& model) {
            setUniform("model", model);
        }

        void setViewMatrix(const glm::mat4& view) {
            setUniform("view", view);
        }

        void setProjectionMatrix(const glm::mat4& projection) {
            setUniform("projection", projection);
        }

        void setObjectColor(const glm::vec4& color) {
            setUniform("objectColor", color);
        }

	};
}

#endif