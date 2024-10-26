
#ifndef FACE_RENDERER_H
#define FACE_RENDERER_H

#include "vertexBuffer.h"
#include "mesh.h"

namespace pe {

    enum class NORMALS {
        USE_VERTEX_NORMALS,
        USE_FACE_NORMALS,
        OMIT
    };

    enum class UV {
        INCLUDE,
        OMIT
    };

    class FaceRenderer {

    private:

        /*
            Determines whether we use the vertex normals or face normals.
            If the shape is curved and they are different, then this would
            be the difference between the shaping having smooth and sharp
            transitions between faces.
        */
        NORMALS useNormals;
        UV useUv;


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


        // The vertex buffer object
        VertexBuffer buffer;


        /*
            Generates face data. Note that it must be triangulated before it is
            returned.
        */
        std::vector<float> generateFaceData() const {

            std::vector<unsigned int> attributeSizes = 
                getAttributeSizes(useNormals, useUv);

            int totalAttributeSize{0};
            for (int i = 0; i < attributeSizes.size(); i++) {
                totalAttributeSize += attributeSizes[i];
            }
            std::vector<float> data(vertexNumber * totalAttributeSize);

            int normalOffset = vertexNumber * 3;
            int uvOffset = 
                (useNormals != NORMALS::OMIT ? vertexNumber * 6 : vertexNumber * 3);

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
                        the triangle vertices will be 0, 1, 2, then 0, 2, 3 etc...
                        So the kth vertex for triangle j is just the vertex i + j.
                    */
                    for (int k = 0; k < 3; k++) {

                        int vertexIndex;
                        if (k == 0) vertexIndex = 0;
                        else vertexIndex = j + k;

                        // The vertex coordinates
                        Vector3D vertex = mesh->getFaceVertex(i, vertexIndex);
                        data[iterations * 3] = vertex.x;
                        data[iterations * 3 + 1] = vertex.y;
                        data[iterations * 3 + 2] = vertex.z;

                        // The normal data
                        if (useNormals != NORMALS::OMIT) {
                            Vector3D normal = (
                                useNormals == NORMALS::USE_VERTEX_NORMALS ?
                                mesh->getVertexNormal(i, vertexIndex) :
                                mesh->getFace(i).getNormal()
                            );

                            data[iterations * 3 + normalOffset] = normal.x;
                            data[iterations * 3 + 1 + normalOffset] = normal.y;
                            data[iterations * 3 + 2 + normalOffset] = normal.z;
                        }


                        // The UV coordinates
                        if (useUv != UV::OMIT) {
                            Vector2D uv = mesh->getFace(i).getTextureCoordinate(vertexIndex);
                            data[iterations * 2 + uvOffset] = uv.x;
                            data[iterations * 2 + 1 + uvOffset] = uv.y;
                        }

                        iterations++;
                    }
                }
            }

            return data;
        }


        /*
            Returns the number of vertices needed to draw the faces of a mesh.
        */
        int getVertexNumber(const Mesh* mesh) {
            int vertexNumber = 0;
            for (int i = 0; i < mesh->getFaceCount(); i++) {
                /*
                    If a face has n vertices, the number of triangles
                    will be n-2, so we will have 3 times that vertices.
                */
                vertexNumber += 3 * (mesh->getFace(i).getVertexCount() - 2);
            }
            return vertexNumber;
        }


        static std::vector<unsigned int> getAttributeSizes(
            NORMALS useNormals,
            UV useUv
        ) {
            if (useNormals != NORMALS::OMIT && useUv != UV::OMIT) {
                return std::vector<unsigned int>{3, 3, 2};
            }
            else if (useNormals == NORMALS::OMIT) {
                return std::vector<unsigned int>{3, 2};
            }
            else if (useUv != UV::OMIT) {
                return std::vector<unsigned int>{3, 3};
            }
            else {
                return std::vector<unsigned int>{3};
            }
        }


    public:

        /*
            The draw type indicated whether ot not the primitive is static
            or dynamic, meaning rigid or deformable (vertices changing often,
            requiring the VAO and the VBO to be edited).
        */
        FaceRenderer(
            const Mesh* mesh,
            GLenum drawType,
            NORMALS useNormals,
            UV useUv
        ) : mesh{ mesh },
            drawType{ drawType }, 
            useNormals{ useNormals },
            useUv{useUv},
            vertexNumber{ getVertexNumber(mesh) },
            buffer(
                vertexNumber,
                getAttributeSizes(useNormals, useUv),
                3,
                drawType
            ) {

            // Sets the data initially
            buffer.setData(generateFaceData());
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
            buffer.setData(generateFaceData());
        }

    };

}

#endif