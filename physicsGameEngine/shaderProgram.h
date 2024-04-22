/*
    Class that handles the compilation and linking of a vertex shader
    and fragment shader, to create a ready to be used shader program.
*/

#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <GL/gl.h>
#include <iostream>

#include <SFML/Graphics.hpp>
#include <glm.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/matrix_transform.hpp>

namespace pe {

	class ShaderProgram {
	private:

        GLuint shaderProgram;

        // Function to compile a shader and return its ID
        GLuint compileShader(GLenum shaderType,
            const std::string& shaderSource) {

            GLuint shaderID = glCreateShader(shaderType);
            const char* source = shaderSource.c_str();
            glShaderSource(shaderID, 1, &source, nullptr);
            glCompileShader(shaderID);

            // Check for compilation errors
            GLint success;
            glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
            if (!success) {
                GLchar infoLog[512];
                glGetShaderInfoLog(shaderID, 512, nullptr, infoLog);
                std::cerr << "Shader compilation error:\n" << infoLog
                    << "\n";
                glDeleteShader(shaderID);
                return 0;
            }

            return shaderID;
        }

        // Function to link a shader program and return its ID
        GLuint linkShaderProgram(GLuint vertexShader, 
            GLuint fragmentShader) {

            GLuint programID = glCreateProgram();
            glAttachShader(programID, vertexShader);
            glAttachShader(programID, fragmentShader);
            glLinkProgram(programID);

            // Check for linking errors
            GLint success;
            glGetProgramiv(programID, GL_LINK_STATUS, &success);
            if (!success) {
                GLchar infoLog[512];
                glGetProgramInfoLog(programID, 512, nullptr, infoLog);
                std::cerr << "Shader program linking error:\n" << infoLog
                    << "\n";
                glDeleteProgram(programID);
                return 0;
            }

            return programID;
        }


        void checkShaderCompilation(GLuint shader) {
            GLint success;
            GLchar infoLog[512];
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(shader, 512, nullptr, infoLog);
                std::cerr << "Shader compilation error:\n" << infoLog
                    << "\n";
            }
        }


        void checkShaderProgramLinking(GLuint shaderProgram) {
            GLint success;
            GLchar infoLog[512];
            glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
                std::cerr << "Shader program linking error:\n" << infoLog
                    << "\n";
            }
        }


        void compileAndLinkShaderProgram(
            const std::string& vertexShaderSource,
            const std::string& fragmentShaderSource
        ) {
            GLuint vertexShader = compileShader(GL_VERTEX_SHADER, 
                vertexShaderSource);
            checkShaderCompilation(vertexShader);
            GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, 
                fragmentShaderSource);
            checkShaderCompilation(fragmentShader);
            checkShaderCompilation(vertexShader);
            checkShaderCompilation(fragmentShader);
            GLuint shaderProgram = linkShaderProgram(vertexShader, 
                fragmentShader);
            checkShaderProgramLinking(shaderProgram);

            this->shaderProgram = shaderProgram;
        }


	public:

		ShaderProgram(
            const std::string& vertexShaderSource,
            const std::string& fragmentShaderSource
        ) {
            compileAndLinkShaderProgram(
                vertexShaderSource,
                fragmentShaderSource
            );
        }

        /*
            Releases shader program resources.
        */
        ~ShaderProgram() {
            glDeleteProgram(shaderProgram);
        }

        // Returns the shader program itself (by reference)
        GLuint getShaderProgram() const {
            return shaderProgram;
        }
	};
}

#endif