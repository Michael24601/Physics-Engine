
#include "tesselationUtil.h"

using namespace pe;

std::vector<Vector3D> pe::generateSphereVertices(const Vector3D& center,
    real radius, int latitudeSegments, int longitudeSegments) {

    std::vector<Vector3D> vertices;
    for (int lat = 0; lat <= latitudeSegments; ++lat) {
        for (int lon = 0; lon <= longitudeSegments; ++lon) {
            // Vertices of the current square
            Vector3D v;

            real phi = 2.0f * PI * static_cast<real>(lon)
                / static_cast<real>(longitudeSegments);
            real theta = PI * static_cast<real>(lat)
                / static_cast<real>(latitudeSegments);

            // Calculate the vertex
            v += Vector3D(
                sin(theta) * cos(phi),
                cos(theta),
                sin(theta) * sin(phi)
            );
            v.normalize();
            v *= radius;
            v += center;

            vertices.push_back(v);
        }
    }
    return vertices;
}


std::vector<std::vector<Vector3D>> pe::returnTesselatedFaces(
    std::vector<Vector3D>& vertices,
    int latitudeSegments,
    int longitudeSegments
) {
    std::vector<std::vector<Vector3D>> faces;

    for (size_t lat = 0; lat < latitudeSegments; ++lat) {
        for (size_t lon = 0; lon < longitudeSegments; ++lon) {
            // Indices of the four vertices of the current triangle
            // in counterclockwise order.
            size_t v0 = lat * (longitudeSegments + 1) + lon;
            size_t v1 = v0 + 1;
            size_t v2 = (lat + 1) * (longitudeSegments + 1) + lon;
            size_t v3 = v2 + 1;

            // Connect vertices to form faces in counter clockwise order
            std::vector<Vector3D> face;
            face = { vertices[v0] , vertices[v1] ,
                vertices[v3], vertices[v2] };
            faces.push_back(face);
        }
    }

    return faces;
}


std::vector<std::pair<Vector3D, Vector3D>> pe::returnTesselatedEdges(
    std::vector<Vector3D>& vertices,
    int latitudeSegments,
    int longitudeSegments
) {
    std::vector<std::pair<Vector3D, Vector3D>> edges;
    for (size_t lat = 0; lat < latitudeSegments; ++lat) {
        for (size_t lon = 0; lon < longitudeSegments; ++lon) {
            // Indices of the four vertices of the current square
            // in counter clockwise order
            size_t v0 = lat * (longitudeSegments + 1) + lon;
            size_t v1 = v0 + 1;
            size_t v2 = (lat + 1) * (longitudeSegments + 1) + lon;
            size_t v3 = v2 + 1;


            // Connect vertices to form edges based on the squares
            std::pair<Vector3D, Vector3D> edge1, edge2, edge3, edge4;
            edge1 = { vertices[v0], vertices[v2] };
            edge2 = { vertices[v3], vertices[v1] };
            edge3 = { vertices[v2], vertices[v3] };
            edge4 = { vertices[v1], vertices[v0] };
            edges.push_back(edge1);
            edges.push_back(edge2);
            edges.push_back(edge3);
            edges.push_back(edge4);
        }
    }

    return edges;
}


std::vector<std::vector<Vector3D*>> pe::returnTesselatedFacesPointers(
    std::vector<Vector3D>& vertices,
    int latitudeSegments,
    int longitudeSegments
) {
    std::vector<std::vector<Vector3D*>> faces;

    for (size_t lat = 0; lat < latitudeSegments; ++lat) {
        for (size_t lon = 0; lon < longitudeSegments; ++lon) {
            // Indices of the four vertices of the current square
            // in counterclockwise order.
            size_t v0 = lat * (longitudeSegments + 1) + lon;
            size_t v1 = v0 + 1;
            size_t v2 = (lat + 1) * (longitudeSegments + 1) + lon;
            size_t v3 = v2 + 1;

            // Connect vertices to form faces in counter clockwise order
            std::vector<Vector3D*> face;
            face = { &vertices[v0] , &vertices[v1] ,
                &vertices[v3], &vertices[v2] };
            faces.push_back(face);
        }
    }

    return faces;
}


std::vector<std::pair<Vector3D*, Vector3D*>> pe::returnTesselatedEdgesPointers(
    std::vector<Vector3D>& vertices,
    int latitudeSegments,
    int longitudeSegments
) {
    std::vector<std::pair<Vector3D*, Vector3D*>> edges;
    for (size_t lat = 0; lat < latitudeSegments; ++lat) {
        for (size_t lon = 0; lon < longitudeSegments; ++lon) {
            // Indices of the four vertices of the current quad/triangle
            size_t v0 = lat * (longitudeSegments + 1) + lon;
            size_t v1 = v0 + 1;
            size_t v2 = (lat + 1) * (longitudeSegments + 1) + lon;
            size_t v3 = v2 + 1;


            // Connect vertices to form edges based on the sqaures
            std::pair<Vector3D*, Vector3D*> edge1, edge2, edge3, edge4;
            edge1 = { &vertices[v0], &vertices[v2] };
            edge2 = { &vertices[v3], &vertices[v1] };
            edge3 = { &vertices[v2], &vertices[v3] };
            edge4 = { &vertices[v1], &vertices[v0] };
            edges.push_back(edge1);
            edges.push_back(edge2);
            edges.push_back(edge3);
            edges.push_back(edge4);
        }
    }

    return edges;
}



