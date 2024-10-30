
#ifndef FACE_BUFFER_GENERATOR_H
#define FACE_BUFFER_GENERATOR_H

#include "vertexBuffer.h"
#include "mesh.h"

namespace pe {


    enum class NORMALS {
        VERTEX_NORMALS,
        FACE_NORMALS,
        OMIT
    };


    enum class UV {
        INCLUDE,
        OMIT
    };

    
    // Returns the vertex number after triangulation
    inline int getVertexNumber(const Mesh* mesh);


    // Returns the attribute sizes given some input settings
    std::vector<unsigned int> getAttributeSizes(
        NORMALS useNormals,
        UV useUv
    );


    // Allocates the data for a VBO meant to render a surface and returns it
    VertexBuffer createFaceVertexBuffer(
        const Mesh* mesh,
        GLenum drawType,
        NORMALS useNormals,
        UV useUv
    );


    // Returns triangulated face data of a mesh
    std::vector<float> generateFaceData(
        const Mesh* mesh,
        NORMALS useNormals,
        UV useUv
    );

}

#endif