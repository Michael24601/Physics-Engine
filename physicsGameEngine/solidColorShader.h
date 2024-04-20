
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

        void setObjectColor(const glm::vec4& color) {
            setUniform("objectColor", color);
        }

	};
}

#endif