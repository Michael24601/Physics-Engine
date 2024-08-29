
#ifndef CUSTOM_PRIMITIVE_H
#define CUSTOM_PRIMITIVE_H

#include "polyhedron.h"
#include "axisAlignedBoundingBox.h"
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
        std::vector<Vector3D>* localVertices,
        const std::vector<Vector3D>& normals,
        const std::vector<Vector2D>& textures,
        std::vector<Face*>& faces,
        std::vector<Edge*>& edges
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


    /*
        This function approximates the inertia tensor.
        Each entry of the inertia tensor is a triple integral over the
        volume of the shape, with the following function being integrated
        I_ij = (mass / volume) * \int_V (x^2 + y^2 + z^2) * delta_ij -r_ir_j dV
        where delat_ij is 1 if i = j, and 0 otherwise, and (x^2 + y^2 + z^2)
        is the magnitude squared of the vector, and r is the vector itself.
        Because it is an integral, we can approximate it using a sum
        I_ij = mass * \sum_k (x^2 + y^2 + z^2) * delta_ij -r_kir_kj
        where k is one point. Done over many points (thousands), this very
        accurately approximates the integral.
        The points will have to be uniformally distributed inside the volume
        of the shape. This means that they will have to be picked at random.
        It's especially easy to pick random points in a rectamgular prism
        aligned with the cartesian plane.
        One startegy will be thus be to choose points at random in the
        bounding box (smallest rectangular prism aligned with the plane
        containing the shape) and taking only those that are in the shape
        (checking if they are in the polyhedron using the monte carlo
        apporach). This works even for convex shapes.
    */
    Matrix3x3 approximateInertiaTensor(Polyhedron* polyhedron);


    Polyhedron returnPrimitive(
        std::string filename,
        real mass,
        const Vector3D& position,
        RigidBody* body,
        real scale
    );

}

#endif