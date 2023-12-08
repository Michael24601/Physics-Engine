
#ifndef SOLID_COLOR_SHADER_H
#define SOLID_COLOR_SHADER_H

#include "shaderProgram.h"
#include "vector3D.h"
#include "drawingUtil.h"
#include "shaderInterface.h"

const std::string solidColorVertexShader = R"(
	#version 330 core

    layout(location = 0) in vec3 aPos;

    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    void main(){
        gl_Position = projection * view * model * vec4(aPos, 1.0);
    }
)";

const std::string solidColorFragmentShader = R"(
	#version 330 core

    uniform vec4 objectColor;
    out vec4 FragColor;

    void main(){
        FragColor = objectColor;
    }
)";

namespace pe {

	class SolidColorShader {
	private:

        ShaderProgram shaderProgramObject;

	public:

        SolidColorShader() : shaderProgramObject(
            solidColorVertexShader,
            solidColorFragmentShader) {}

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