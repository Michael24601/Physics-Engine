
#ifndef EDGE_RENDERER_H
#define EDGE_RENDERER_H

#include "vertexBuffer.h"
#include "mesh.h"

namespace pe {

    class EdgeRenderer {

    private:

        // Pointer to the mesh being rendered
        const Mesh* mesh;


        // Static or dynamic for rigid and deformable objects 
        GLenum drawType;

        /*
            Determines whether we use the vertex normals or face normals.
            If the shape is curved and they are different, then this would
            be the difference between the shaping having smooth and sharp
            transitions between faces.
        */
        bool smooth;

        VertexBuffer buffer;

        std::vector<float> generateEdgeData() const {

            // Edge data, containing two vertices, each 3 floats
            std::vector<float> data(mesh->getEdgeCount() * 6);

            for (int i = 0; i < mesh->getEdgeCount(); i++) {

                Vector3D vertex1 = mesh->getEdgeVertex(i, 0);
                Vector3D vertex2 = mesh->getEdgeVertex(i, 1);

                data[i * 6] = vertex1.x;
                data[i * 6 + 1] = vertex1.y;
                data[i * 6 + 2] = vertex1.z;
                data[i * 6 + 3] = vertex2.x;
                data[i * 6 + 4] = vertex2.y;
                data[i * 6 + 5] = vertex2.z;
            }

            return data;
        }


    public:


        /*
            The draw type indicated whether ot not the primitive is static
            or dynamic, meaning rigid or deformable (vertices changing often,
            requiring the VAO and the VBO to be edited).
        */
        EdgeRenderer(
            const Mesh* mesh,
            GLenum drawType,
            bool smooth = true
        ) : mesh{ mesh }, drawType{ drawType }, smooth{ smooth },
            /*
                The edge vertices have 1 attribute: the coordinate.
                There are 2 vertices per edge, so the vertex number is
                2 times the number of edges.
                The topology type is a line, which is 2 (for 2 vertices).
            */
            buffer(
                mesh->getEdgeCount() * 2,
                std::vector<unsigned int>{3},
                2,
                drawType
            ) {
            
            // Sends the data initially
            buffer.setData(generateEdgeData());
        }


        const VertexBuffer& getVertexBuffer() const {
            return buffer;
        }


        /*
            This function modifies the VBO objects by updating them.
            The function can be called whether the draw type is static or
            dynamic, though it's obviously preferable it be dynamic if the
            function is called a lot.
        */
        void updateVbo() {
            buffer.setData(generateEdgeData());
        }

    };

}

#endif