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

        GLuint vao;
        GLuint vbo;

        GLsizei triangleNumber;
        GLsizei edgeNumber;

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
        ), vao(0), vbo(0), triangleNumber(0), edgeNumber(0) {

            glGenVertexArrays(1, &vao);
            glGenBuffers(1, &vbo);
        }


        ~Shader() {
            // Deletes the VAO and VBO and removes the program
            cleanUpVAOandVBO();
            shaderProgram.~ShaderProgram();
            glUseProgram(0);
        }


        /*
            Sends the variable data to the shader program. That includes
            vertices, normals, tangents, bitangents, and texture coordinates.
            This function should be called only once at the start of the
            scene, not each frame, since the normals and vertices and whatnot
            don't need to change between frames; if they are sent as local
            coordinates initially, the model matrix uniform can be used to
            update them each frame instead, saving time.
            On the other hand, some diformable shapes like particle meshes
            don't have transform matrices, only global vertices, which need
            to be sent each frame.

            If they are updated each frame however, they need to be deleted
            first as to not overrun the memory (using cleanUpVAOandVBO()).

            Notice that only vec3 objects are accepted. So if we have any
            vec2 coordinates for example, such as uv coordinates,
            we can just cast the, to vec3 first. They will be cast back to
            vec2 in the shader.
            
            The draw type is sent to tell the OpenGL if the data will be sent
            once at initialization (STATIC_DRAW), or if it will be sent
            once per frame (DYNAMIC_DRAW) in the case of deformable bodies.
        */
        void sendVaribleData(
            const std::vector<std::vector<glm::vec3>>& data,
            GLenum drawType
        ) {

            glBindVertexArray(vao);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);

            // Clearing the VBO
            glBufferData(GL_ARRAY_BUFFER, 0, nullptr, drawType);

            unsigned int totalSize = 0;
            for (int i = 0; i < data.size(); i++) {
                totalSize += data[i].size();
            }

            size_t dataSize = totalSize * sizeof(glm::vec3);

            // Buffer that holds all of the data
            std::vector<glm::vec3> combinedData;
            combinedData.reserve(totalSize);
            for (int i = 0; i < data.size(); i++) {
                for (int j = 0; j < data[i].size(); j++) {
                    combinedData.push_back(data[i][j]);
                }
            }

            // The combined data is uploaded to the VBO
            glBufferData(
                GL_ARRAY_BUFFER,
                dataSize,
                combinedData.data(),
                /*
                    Here, we use static draw, because we expect the geometry
                    to remain the same most of the time. Even if the objects
                    move and rotate, the vertices remain the same (the model
                    matrix can transform them).
                */
                drawType
            );

            GLenum error;
            while ((error = glGetError()) != GL_NO_ERROR) {
                std::cerr << "OpenGL error: " << error << std::endl;
            }

            size_t incrementalSize{0};
            for (int i = 0; i < data.size(); i++) {
                glVertexAttribPointer(
                    i, 3,
                    GL_FLOAT,
                    GL_FALSE,
                    sizeof(glm::vec3),
                    (void*)(incrementalSize * sizeof(glm::vec3))
                );

                glEnableVertexAttribArray(i);

                incrementalSize += data[i].size();
            }

            // Unbinds VAO
            glBindVertexArray(0);

            // Cleans up the VBO
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }


        /*
            When we send our variable data, some of it will refer to normals,
            other to tangents, and others to vertices. The number of vertex
            data is needed when drawing the shapes as it will be partitioned
            into threes, which are used to draw triangles.
        */
        void setTrianglesNumber(int size) {
            this->triangleNumber = size;
        }


        void setEdgeNumber(int size) {
            this->edgeNumber = size;
        }

        
        GLuint getShaderProgram() {
            return shaderProgram.getShaderProgram();
        }


        void cleanUpVAOandVBO() {
            if (vao != 0) {
                glDeleteVertexArrays(1, &vao);
                vao = 0;
            }
            if (vbo != 0) {
                glDeleteBuffers(1, &vbo);
                vbo = 0;
            }
        }


        /*
            Function that draws faces (assuming the vertices that were
            sent to it in the vao correspond to triangles).
        */
        void drawFaces() {

            glUseProgram(shaderProgram.getShaderProgram());

            // Binds VAO and draw
            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, triangleNumber);
            glBindVertexArray(0);

            GLenum error = glGetError();
            if (error != GL_NO_ERROR) {
                std::cerr << "OpenGL Error: " << error << "\n";
            }

            /*
                 Goes back to using no program,(in case other shaders need
                 to draw).
             */
            glUseProgram(0);
        }

        /*
            Function that draws edges (assuming the vertices that were
            sent to it in the vao correspond to segments).
        */
        void drawEdges() {
            // Uses our program
            glUseProgram(shaderProgram.getShaderProgram());

            // Binds VAO and draw
            glBindVertexArray(vao);
            glDrawArrays(GL_LINES, 0, edgeNumber);
            glBindVertexArray(0);

            GLenum error = glGetError();
            if (error != GL_NO_ERROR) {
                std::cerr << "OpenGL Error: " << error << "\n";
            }

            /*
                Goes back to using no program,(in case other shaders need
                to draw).
            */
            glUseProgram(0);
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