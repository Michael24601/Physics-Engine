
#ifndef EDGE_BUFFER_GENERATOR_H
#define EDGE_BUFFER_GENERATOR_H

#include "vertexBuffer.h"
#include "mesh.h"

namespace pe {

    // Allocates the data for a VBO meant to render a line and returns it
    VertexBuffer createEdgeVertexBuffer(
        const Mesh* mesh,
        GLenum drawType
    );

    // Generates the edge data of a mesh
    std::vector<float> generateEdgeData(const Mesh* mesh);

}

#endif