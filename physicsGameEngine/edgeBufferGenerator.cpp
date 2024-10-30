
#include "edgeBufferGenerator.h"

using namespace pe;


VertexBuffer pe::createEdgeVertexBuffer(
    const Mesh* mesh,
    GLenum drawType
) {
    return VertexBuffer(
        mesh->getEdgeCount() * 2,
        std::vector<unsigned int>{3},
        2,
        drawType
    );
}


std::vector<float> pe::generateEdgeData(const Mesh* mesh) {

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
