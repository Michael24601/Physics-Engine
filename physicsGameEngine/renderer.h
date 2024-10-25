
#ifndef RENDERER_H
#define RENDERER_H

#include "vertexBuffer.h"
#include "mesh.h"

namespace pe {

    class Renderer {

    private:

        // Object used for rendering
        VertexBuffer* edgeRenderer;
        VertexBuffer* faceRenderer;


        // Pointer to the mesh being rendered
        const Mesh* mesh;


        // Static or dynamic for rigid and deformable objects 
        GLenum drawType;

        /*
            The sum of the number of vertices in each face in the mesh
            after triangulation, which is calculated in the constructor
            after the mesh is sent.
        */
        int vertexNumber;

        /*
            Determines whether we use the vertex normals or face normals.
            If the shape is curved and they are different, then this would
            be the difference between the shaping having smooth and sharp
            transitions between faces.
        */
        bool smooth;


        /*
            Generates face data. Note that it must be triangulated before it is
            returned.
        */
        std::vector<float> generateFaceData() const {

            /*
                Face data, containing 3 floats for each vertex,
                3 floats for each normal, and 2 floats for each UV coordinate.
            */
            std::vector<float> data(vertexNumber * 8);

            // The number of iterations
            int iterations{ 0 };

            // For each face
            for (int i = 0; i < mesh->getFaceCount(); i++) {
                /*
                    For each triangle in each face, of which there are n - 2
                    for a polygon with n vertices.
                */
                for (int j = 0; j < mesh->getFace(i).getVertexCount() - 2; j++) {
                    /*
                        For each vertex in each triangle.
                        Since we know the vertices in the face are counter clockwise,
                        the triangle vertices will be 0, 1, 2, then 1, 2, 3 etc...
                        So the kth vertex for triangle j is just the vertex i + j.
                    */
                    for (int k = 0; k < 3; k++) {

                        int vertexIndex = j + k;

                        // The vertex coordinates
                        Vector3D vertex = mesh->getFaceVertex(i, vertexIndex);
                        data[iterations * 3] = vertex.x;
                        data[iterations * 3 + 1] = vertex.y;
                        data[iterations * 3 + 2] = vertex.z;
                        
                        Vector3D normal = (
                            smooth ? 
                            mesh->getVertexNormal(i, vertexIndex) : 
                            mesh->getFace(i).getNormal()
                        );

                        data[iterations * 3 + vertexNumber] = normal.x;
                        data[iterations * 3 + vertexNumber + 1] = normal.y;
                        data[iterations * 3 + vertexNumber + 2] = normal.z;
     

                        // The UV coordinates
                        Vector2D uv = mesh->getFace(i).getTextureCoordinate(vertexIndex);
                        data[iterations * 2 + vertexNumber * 2] = uv.x;
                        data[iterations * 2 + vertexNumber * 2 + 1] = uv.x;

                        iterations++;
                    }
                }
            }

            return data;
        }


        std::vector<float> generateEdgeData() const {

            // Edge data, containing two vertices, each 3 floats
            std::vector<float> data(mesh->getEdgeCount() * 6);

            for (int i = 0; i < mesh->getEdgeCount(); i++) {

                Vector3D vertex1 = mesh->getEdgeVertex(i, 0);
                Vector3D vertex2 = mesh->getEdgeVertex(i, 1);

                data[i * 6] = vertex1.x;
                data[i * 6 + 1] = vertex1.y;
                data[i * 6 + 2] = vertex1.z;
                data[i * 6 + 3] = vertex1.x;
                data[i * 6 + 4] = vertex1.y;
                data[i * 6 + 5] = vertex1.z;
            }

            return data;
        }


    public:


        /*
            The draw type indicated whether ot not the primitive is static
            or dynamic, meaning rigid or deformable (vertices changing often,
            requiring the VAO and the VBO to be edited).
        */
        Renderer(
            const Mesh* mesh,
            GLenum drawType,
            bool smooth = true
        ) : mesh{ mesh }, drawType{ drawType }, smooth{ smooth } {

            vertexNumber = 0;
            for (int i = 0; i < mesh->getFaceCount(); i++) {
                /*
                    If a face has n vertices, the number of triangles
                    will be n-2, so we will have 3 times that vertices.
                */
                vertexNumber += 3 * (mesh->getFace(i).getVertexCount() - 2);
            }
            
            /*
                The edge vertices have 1 attribute: the coordinate.
                There are 2 vertices per edge, so the vertex number is
                2 times the number of edges.
                The topology type is a line, which is 2 (for 2 vertices).
            */
            edgeRenderer = new VertexBuffer(
                mesh->getEdgeCount() * 2,
                std::vector<unsigned int>{3},
                2, 
                drawType
            );

            /*
                 The face vertices have 3 attributes: the coordinate, normal,
                 and uv coordinate.
                 The number of vertices is the sum of the number of vertices
                 in each face after triangulation.
                 The topology type is a triangle, which is 3 (for 3 vertices).
            */
            faceRenderer = new VertexBuffer(
                vertexNumber, 
                std::vector<unsigned int>{3, 3, 2},
                3,
                drawType
            );
        }


        /*
            This function modifies the VBO objects by updating them.
            The function can be called whether the draw type is static or
            dynamic, though it's obviously preferable it be dynamic if the
            function is called a lot.
        */
        void updateVbo() {
            edgeRenderer->setData(generateEdgeData());
            faceRenderer->setData(generateFaceData());
        }


        ~Renderer() {
            delete edgeRenderer;
            delete faceRenderer;
        }

    };

}

#endif