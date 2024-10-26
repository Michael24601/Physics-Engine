
#ifndef IMPORTED_MESH_H
#define IMPORTED_MESH_H

#include "mesh.h"
#include "boundingBox.h"
#include "util.h"
#include <string>

namespace pe {

    /*
        A face in an OBJ file always has a vertex first,
        followed by a texture coordinate, a vertex normal,
        or both.
        If each vertex does not have a normal,
        the the face is flat, and only has a single normal
        which does not need to be specified as it can be
        calculated from the vertices.
    */
    enum class FaceType {
        VertexOnly,
        VertexNormal,
        VertexTexture,
        VertexNormalTexture
    };

    // Function to remove spaces at the start of a string
    std::string removeSpacesAtStartOfSpring(std::string str);


    int countOccurrences(const std::string& str, const std::string& substr);


    // Function to determine the type of face
    static FaceType determineFaceType(const std::string& faceStr);


    static void extractMeshInformation(
        const std::string& filename,
        std::vector<Vector3D>& vertices,
        std::vector<Vector3D>& normals,
        std::vector<Vector2D>& textures
    );


    static void extractMeshFacesAndEdges(
        const std::string& filename,
        const std::vector<Vector3D>& vertices,
        const std::vector<Vector3D>& normals,
        const std::vector<Vector2D>& textures,
        std::vector<std::vector<int>>& faces,
        std::vector<std::pair<int, int>>& edges,
        std::vector<std::vector<Vector3D>>& vertexNormals,
        std::vector<std::vector<Vector2D>>& faceTextureCoords,
        std::vector<std::string>& faceTextureId
    );


    /*
        Since the vertices we get from the OBJ file will be used as the
        local (relagtive) vertices of the primitive we get,
        we find it useful to shift them such that their centre of gravity
        is at the origin
        (that way, the local vertices of the shape could become
        the distance to the vertex from the centre of the object).
    */
    void centerOfGravityToOrigin(std::vector<Vector3D>& vectors);


    Mesh* extractMesh(std::string filename, real scale);

}

#endif