
#ifndef VERTEX_BUFFER_H
#define VERTEX_BUFFER_H

#include "mesh.h"
#include "shaderInterface.h"
#include "shaderProgram.h"

namespace pe {

	class VertexBuffer {

	private:

        void checkForOpenGLError() const {
            GLenum error;
            while ((error = glGetError()) != GL_NO_ERROR) {
                std::cerr << "OpenGL error: " << error << std::endl;
            }
        }

        /*
            Given the number of attributes (such as vertex, normal, uv etc...)
            and the number of vertices, and the size of each vertex
            (vec3, vec4...), and the draw type (static or dynamic) this function
            allocated the memory on the GPU, to be filled later on.
        */
        void allocateBuffer() {

            // Generating VAO and VBO
            glGenVertexArrays(1, &vao);
            glGenBuffers(1, &vbo);

            // Binding the VAO and VBO
            glBindVertexArray(vao);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);

            // Allocating the buffer space without uploading any data
            glBufferData(GL_ARRAY_BUFFER, totalDataSize, nullptr, drawType);

            // Error checking
            checkForOpenGLError();

            size_t offset = 0;
            for (int i = 0; i < attributeNumber; i++) {
                glVertexAttribPointer(
                    i,
                    attributeSizes[i],
                    GL_FLOAT,
                    GL_FALSE,
                    sizeof(float) * attributeSizes[i],
                    (void*)(offset)
                );

                glEnableVertexAttribArray(i);

                offset += vertexNumber * sizeof(float) * attributeSizes[i];
            }

            // Unbinding the VAO
            glBindVertexArray(0);

            // Cleaning up the VBO
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }


	public:

        // Static or dynamic drawing type, for deformable and rigid meshes
        GLenum drawType;

		// For faces
		GLuint vao;
		GLuint vbo;

        /*
            The size of each attribute, meaning whether it is a vec2, vec3,
            or vec4.
        */
        const std::vector<unsigned int> attributeSizes;

        // The total size of the data
        size_t totalDataSize;

        const unsigned int attributeNumber;
        // Vertex number per attribute
        const GLsizei vertexNumber;

        /*
            Determines if a face (triangle) or edge (line) is being drawn.
            Its value is 3 (triangle) or 2 (edge), and refers to how the
            vertices are packed, in threes or twos.
        */
        unsigned int topologyType;

        VertexBuffer(
            int vertexNumber,
            const std::vector<unsigned int>& attributeSizes,
            unsigned int topologyType,
            GLenum drawType
        ) : drawType{ drawType }, 
            attributeSizes(attributeSizes),
            attributeNumber{attributeSizes.size()},
            vertexNumber{vertexNumber},
            topologyType{ topologyType }, 
            vao{ 0 }, 
            vbo{0} {

            // Ensuring the topology type is acceptable
            if (topologyType != 2 && topologyType != 3) {
                throw std::invalid_argument(
                    "topologyType must be 2 (edges) or 3 (triangles)"
                );
            }

            totalDataSize = 0;
            for (int i = 0; i < attributeNumber; i++) {
                totalDataSize += vertexNumber * sizeof(float) * attributeSizes[i];
            }
            
            allocateBuffer();
        }


        ~VertexBuffer() {
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
            Function to modify data in the existing VBO.
            The attribute index always starts at 0.
        */
        void setAttributeData(
            int attributeIndex, 
            const std::vector<float>& attributeData
        ) {
            glBindBuffer(GL_ARRAY_BUFFER, vbo);

            if (attributeIndex >= attributeNumber || attributeIndex < 0) {
                throw std::invalid_argument(
                    "The attribute index is out of bounds"
                );
            }

            if (attributeData.size() * sizeof(float) != totalDataSize) {
                throw std::invalid_argument(
                    "The number of data points must match the number of vertices"
                );
            }

            // Moving past previous attributes
            int offset = 0;
            for (int i = 0; i < attributeIndex; i++) {
                offset += vertexNumber * sizeof(float) * attributeSizes[i];
            }

            // The data size for the current attribute
            int dataSize = 
                vertexNumber * sizeof(float) * attributeSizes[attributeIndex];

            // Updating the attribute data at the specific offset
            glBufferSubData(GL_ARRAY_BUFFER, offset, dataSize, attributeData.data());

            // Unbinding the VBO
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            // Error check
            checkForOpenGLError();
        }


        /*
            Becasuse repeatedly binding and unbinding the VBO can be
            inefficient, so we can set the entire VBO data here.
        */
        void setData(const std::vector<float>& combinedData) {

            if (combinedData.size() * sizeof(float) != totalDataSize) {
                throw std::invalid_argument(
                    "The combined data size must be attributeNumber * vertexNumber"
                );
            }

            glBindBuffer(GL_ARRAY_BUFFER, vbo);

            // Uploading all the combined attribute data at once
            glBufferSubData(GL_ARRAY_BUFFER, 0, totalDataSize, combinedData.data());

            glBindBuffer(GL_ARRAY_BUFFER, 0);

            // Error check
            checkForOpenGLError();
        }


        /*
            Renders the VAO content.
            Note that this function must be called after a shader program
            has been activated/used.
        */
        void render() const {

            // Binds VAO and draw
            glBindVertexArray(vao);

            // Triangles
            if (topologyType == 3) {
                glDrawArrays(GL_TRIANGLES, 0, vertexNumber);
            }
            // Lines
            else if (topologyType == 2) {
                glDrawArrays(GL_LINES, 0, vertexNumber);
            }

            // Unbinds the VAO
            glBindVertexArray(0);

            // Error checking
            checkForOpenGLError();
        }

	};
}

#endif