/*
    A vertex buffer used specifically with skyboxes.
*/

#ifndef SKYBOX_VERTEX_BUFFER_H
#define SKYBOX_VERTEX_BUFFER_H

#include "vertexBuffer.h"

namespace pe {

    /*
        The vertices of a unit cube, triangulated, and ordered
        in reverse (clockwise) so that the box faces are draw
        on the inside, not outside, which is what we need for a skybox.
    */
    const static std::vector<float> skyboxVertices{
        // Front face
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        // Back face
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,

         // Left face
         -1.0f,  1.0f,  1.0f,
         -1.0f, -1.0f,  1.0f,
         -1.0f, -1.0f, -1.0f,
         -1.0f, -1.0f, -1.0f,
         -1.0f,  1.0f, -1.0f,
         -1.0f,  1.0f,  1.0f,

         // Right face
          1.0f,  1.0f, -1.0f,
          1.0f, -1.0f, -1.0f,
          1.0f, -1.0f,  1.0f,
          1.0f, -1.0f,  1.0f,
          1.0f,  1.0f,  1.0f,
          1.0f,  1.0f, -1.0f,

          // Top face
          -1.0f,  1.0f,  1.0f,
          -1.0f,  1.0f, -1.0f,
           1.0f,  1.0f, -1.0f,
           1.0f,  1.0f, -1.0f,
           1.0f,  1.0f,  1.0f,
          -1.0f,  1.0f,  1.0f,

          // Bottom face
          -1.0f, -1.0f, -1.0f,
          -1.0f, -1.0f,  1.0f,
           1.0f, -1.0f,  1.0f,
           1.0f, -1.0f,  1.0f,
           1.0f, -1.0f, -1.0f,
          -1.0f, -1.0f, -1.0f
    };

    class SkyboxRenderer {

    private:

        VertexBuffer buffer;

    public:

        SkyboxRenderer(float scale) : 
            buffer(108, std::vector<unsigned int>{3}, 3, GL_STATIC_DRAW) {
            buffer.setData(skyboxVertices);
        }

        const VertexBuffer& getVertexBuffer() const {
            return buffer;
        }
    };
}

#endif