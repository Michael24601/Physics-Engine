/*
	An interface that abstracts away the functionalities of shaders.
    Specific shaders will have to extend this class, and provide extra
    functionalities using the functions provided here, such as allowing
    the setting of uniforms specific to them, or setting textures if
    they use textures at all.
*/

#ifndef SHADER_H
#define SHADER_H

#include "shaderProgram.h"
#include "vector3D.h"
#include "drawingUtil.h"
#include "shaderInterface.h"
#include "openglUtility.h"

namespace pe {

	class Shader {

    protected:

        // Coded into the shader, as it must be known at compile time
        static constexpr int MAXIMUM_NUMBER_OF_LIGHT_SOURCES = 10;

        ShaderProgram shaderProgram;

        /*
            Below are functions that allow us to set uniforms of any type.
        */

        void setUniform(const std::string& name, const glm::mat4& value) {
            glUseProgram(shaderProgram.getShaderProgram());
            GLint location = glGetUniformLocation(shaderProgram.getShaderProgram(), name.c_str());
            if (location != -1) {
                glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
            }
            else {
                // Handle error: Uniform location not found
                std::cerr << "Uniform '" << name << "' not found in shader program.\n";
            }
            glUseProgram(0);
        }

        void setUniform(const std::string& name, const glm::vec3& value) {
            glUseProgram(shaderProgram.getShaderProgram());
            GLint location = glGetUniformLocation(shaderProgram.getShaderProgram(), name.c_str());
            if (location != -1) {
                glUniform3fv(location, 1, glm::value_ptr(value));
            }
            else {
                // Handle error: Uniform location not found
                std::cerr << "Uniform '" << name << "' not found in shader program.\n";
            }
            glUseProgram(0);
        }

        void setUniform(const std::string& name, const glm::vec4& value) {
            glUseProgram(shaderProgram.getShaderProgram());
            GLint location = glGetUniformLocation(shaderProgram.getShaderProgram(), name.c_str());
            if (location != -1) {
                glUniform4fv(location, 1, glm::value_ptr(value));
            }
            else {
                // Handle error: Uniform location not found
                std::cerr << "Uniform '" << name << "' not found in shader program.\n";
            }
            glUseProgram(0);
        }

        void setUniform(const std::string& name, const glm::vec3* arr, int size) {
            glUseProgram(shaderProgram.getShaderProgram());
            GLint location = glGetUniformLocation(shaderProgram.getShaderProgram(), name.c_str());
            if (location != -1) {
                glUniform3fv(location, size, glm::value_ptr(arr[0]));
            }
            else {
                // Handle error: Uniform location not found
                std::cerr << "Uniform '" << name << "' not found in shader program.\n";
            }
            glUseProgram(0);
        }

        void setUniform(const std::string& name, const glm::vec4* arr, int size) {
            glUseProgram(shaderProgram.getShaderProgram());
            GLint location = glGetUniformLocation(shaderProgram.getShaderProgram(), name.c_str());
            if (location != -1) {
                glUniform4fv(location, size, glm::value_ptr(arr[0]));
            }
            else {
                // Handle error: Uniform location not found
                std::cerr << "Uniform '" << name << "' not found in shader program.\n";
            }
            glUseProgram(0);
        }

        void setUniform(const std::string& name, float value) {
            glUseProgram(shaderProgram.getShaderProgram());
            GLint location = glGetUniformLocation(shaderProgram.getShaderProgram(), name.c_str());
            if (location != -1) {
                glUniform1f(location, value);
            }
            else {
                // Handle error: Uniform location not found
                std::cerr << "Uniform '" << name << "' not found in shader program.\n";
            }
            glUseProgram(0);
        }

        void setUniform(const std::string& name, int value) {
            glUseProgram(shaderProgram.getShaderProgram());
            GLint location = glGetUniformLocation(shaderProgram.getShaderProgram(), name.c_str());
            if (location != -1) {
                glUniform1i(location, value);
            }
            else {
                // Handle error: Uniform location not found
                std::cerr << "Uniform '" << name << "' not found in shader program. \n";
            }
            glUseProgram(0);
        }


        /*
            A function that sets a texture, be it a cube map or a normal
            2D texture.
        */
        void setTextureUniform(
            const std::string& name, 
            GLuint textureId, 
            GLenum textureType, 
            GLuint textureUnit
        ) {

            glUseProgram(shaderProgram.getShaderProgram());

            glActiveTexture(GL_TEXTURE0 + textureUnit);
            glBindTexture(textureType, textureId);
            
            GLint location = glGetUniformLocation(
                shaderProgram.getShaderProgram(), 
                name.c_str()
            );
            if (location != -1) {
                glUniform1i(location, textureUnit);
            }
            else {
                std::cerr << "Sampler uniform '" << name 
                    << "' not found in shader program.\n";
            }

            /*
                We then always reset to use texture unit 0 after setting
                the texture.
            */
            glActiveTexture(GL_TEXTURE0);

            glUseProgram(0);
        }


    public:

        Shader(
            const std::string& vertexShaderSource,
            const std::string& fragmentShaderSource
        ) : shaderProgram(
            readFileToString(vertexShaderSource),
            readFileToString(fragmentShaderSource)
        ) {}


        ~Shader() {
            shaderProgram.~ShaderProgram();
            glUseProgram(0);
        }

        
        GLuint getShaderProgram() {
            return shaderProgram.getShaderProgram();
        }


        /*
            The below three functions are public setters for the 3 matrices
            used to transform vertices in the vertex shaders; the model,
            view, and projection.
            Other more specific uniforms such as color, alpha value,
            lightSource etc... are specific to certain shaders, and their
            setters are included in the subclasses. The 3 matrix setters
            are included in the parent class because they are the only
            uniforms guaranteed to be present in every shader.
        */

        void setModelMatrix(const glm::mat4& model) {
            setUniform("model", model);
        }

        void setViewMatrix(const glm::mat4& view) {
            setUniform("view", view);
        }

        void setProjectionMatrix(const glm::mat4& projection) {
            setUniform("projection", projection);
        }

	};
}

#endif