
#include "faceBufferGenerator.h"

using namespace pe;


inline int pe::getVertexNumber(const Mesh* mesh) {
    /*
        When triangulated, each face with n vertices will
        turn into (n-2) triangles each with 3 vertices.
        So we will have Sum((n_i - 2) * 3) where n_i is the number
        of vertices in face_i, which is equal to
        3 * (Sum(n_i) - Sum(2)).
    */
    return (mesh->getFaceVertexCount() - 2 * mesh->getFaceCount()) * 3;
}


std::vector<unsigned int> pe::getAttributeSizes(
    NORMALS useNormals,
    UV useUv
) {
    if (useNormals != NORMALS::OMIT && useUv != UV::OMIT) {
        return std::vector<unsigned int>{3, 3, 2};
    }
    else if (useNormals != NORMALS::OMIT) {
        return std::vector<unsigned int>{3, 3};
    }
    else if (useUv != UV::OMIT) {
        return std::vector<unsigned int>{3, 2};
    }
    else {
        return std::vector<unsigned int>{3};
    }
}


VertexBuffer pe::createFaceVertexBuffer(
    const Mesh* mesh,
    GLenum drawType,
    NORMALS useNormals,
    UV useUv
) {
    return VertexBuffer(
        getVertexNumber(mesh),
        getAttributeSizes(useNormals, useUv),
        3,
        drawType
    );
}


std::vector<float> pe::generateFaceData(
    const Mesh* mesh,
    NORMALS useNormals,
    UV useUv
) {

    int vertexNumber = getVertexNumber(mesh);

    std::vector<unsigned int> attributeSizes =
        getAttributeSizes(useNormals, useUv);

    int totalAttributeSize{ 0 };
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
                        useNormals == NORMALS::VERTEX_NORMALS ?
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