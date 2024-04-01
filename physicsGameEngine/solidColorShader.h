
#ifndef SOLID_COLOR_SHADER_H
#define SOLID_COLOR_SHADER_H

#include "shaderProgram.h"
#include "vector3D.h"
#include "drawingUtil.h"
#include "shaderInterface.h"
#include "openglUtility.h"

namespace pe {

	class SolidColorShader {
	private:

        ShaderProgram shaderProgramObject;

	public:

        SolidColorShader() : shaderProgramObject(
            readFileToString("solidColorVertexShader.glsl"),
            readFileToString("solidColorFragmentShader.glsl")
        ) {}

        void drawEdges(
            const std::vector<glm::vec3>& edges,
            const glm::mat4& model,
            const glm::mat4& view,
            const glm::mat4& projection,
            const glm::vec4& objectColor 
        );

        void drawFaces(
            const std::vector<glm::vec3>& faces,
            const glm::mat4& model,
            const glm::mat4& view,
            const glm::mat4& projection,
            const glm::vec4& objectColor
        );
	};
}

#endif