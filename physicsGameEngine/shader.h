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
#include "vertexBuffer.h"

namespace pe {

    // Forward declaration to avoid circular dependency
    class RenderComponent;

	class Shader {

    protected:

        // Coded into the shader, as it must be known at compile time
        static constexpr int MAXIMUM_NUMBER_OF_LIGHT_SOURCES = 10;

        ShaderProgram shaderProgram;

        /*
            The minimum number of attributes the shader expects
            and the size of each attribute (vec2, vec3, vec4...).
            These can be used to ensure the VBO object sent actually
            has the needed data.
        */
        const unsigned int minAttributeNumber;
        const std::vector<unsigned int> attributeSizes;

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

        void setUniform(const std::string& name, const glm::mat4* value, int size) {
            glUseProgram(shaderProgram.getShaderProgram());
            GLint location = glGetUniformLocation(shaderProgram.getShaderProgram(), name.c_str());
            if (location != -1) {
                glUniformMatrix4fv(location, size, GL_FALSE, glm::value_ptr(value[0]));
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
            /*
                The vertex and fragment shader code,
                as well as the attribute sizes.
            */
            const std::vector<unsigned int>& attributeSizes,
            const std::string& vertexShaderSource,
            const std::string& fragmentShaderSource,
            const std::string& geometryShaderSource = ""
        ) : shaderProgram(
            readFileToString(vertexShaderSource),
            readFileToString(fragmentShaderSource),
            (geometryShaderSource == "" ? "" : readFileToString(geometryShaderSource))
        ), minAttributeNumber{attributeSizes.size()},
           attributeSizes{attributeSizes} {}


        ~Shader() {
            shaderProgram.~ShaderProgram();
            glUseProgram(0);
        }

        
        GLuint getShaderProgram() {
            return shaderProgram.getShaderProgram();
        }


        void useShaderProgram() const {
            glUseProgram(shaderProgram.getShaderProgram());
        }


        /*
            Because each shaders has its own data, each shader will
            override this function and set their own uniforms given a render
            object.
            Note that this only includes the object specific data, not the
            world parameters such as light positions, shininess factors,
            and shadow maps of the world; this data is not available in
            any render component.
        */
        virtual void setObjectData(RenderComponent& component) = 0;


        void setModelMatrix(const glm::mat4& model) {
            setUniform("model", model);
        }

        void setViewMatrix(const glm::mat4& view) {
            setUniform("view", view);
        }

        void setProjectionMatrix(const glm::mat4& projection) {
            setUniform("projection", projection);
        }


        /*
            Funcion that renders the given VBO object using the shader.
            Just ensure the uniforms are set before the function is called.
            The function can be overriden if specific shaders require
            some extra steps.
        */
        virtual void render(const VertexBuffer& buffer) const {

            // Ensuring the VBO is valid
            if (buffer.attributeNumber < minAttributeNumber) {
                std::cerr << "The VBO does not have the required data for this shader."
                    << "(expected at least " << minAttributeNumber 
                    << " attributes, but got " << buffer.attributeNumber << ")\n";
                return;
            }
            for (int i = 0; i < minAttributeNumber; i++) {
                if (buffer.attributeSizes[i] != attributeSizes[i]) {
                    std::cerr << "Attribute size mismatch for attribute index "
                        << i << ". Expected " << attributeSizes[i]
                        << " but got " << buffer.attributeSizes[i] << "\n";
                    return;
                }
            }

            glUseProgram(shaderProgram.getShaderProgram());

            buffer.render();

            /*
                 Goes back to using no program,(in case other shaders need
                 to draw).
             */
            glUseProgram(0);
        }

	};
}

#endif