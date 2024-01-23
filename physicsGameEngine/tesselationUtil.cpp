
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
    const std::vector<Vector3D>& vertices,
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
    const std::vector<Vector3D>& vertices,
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


std::vector<Vector3D> pe::generateCylinderVertices(const Vector3D& center,
    real radius, real height, int segments) {

    std::vector<Vector3D> vertices;

    for (int i = 0; i <= segments; ++i) {
        // Calculate the angle for each segment
        real theta = 2.0f * PI * static_cast<real>(i) / static_cast<real>(segments);

        // Vertices of the current point on the side of the cylinder
        Vector3D v_side;
        v_side += Vector3D(
            radius * cos(theta),
            height / 2.0f,
            radius * sin(theta)
        );
        v_side += center;

        // Vertices of the corresponding point at the bottom of the cylinder
        Vector3D v_bottom = v_side - Vector3D(0.0f, height, 0.0f);

        vertices.push_back(v_side);
        vertices.push_back(v_bottom);
    }

    return vertices;
}


std::vector<std::vector<Vector3D>> pe::returnCylinderFaces(
    const std::vector<Vector3D>& vertices,
    int segments
) {
    std::vector<std::vector<Vector3D>> faces;

    // Add top face
    std::vector<Vector3D> top_face;
    for (size_t i = 0; i < segments; ++i) {
        size_t v0 = 2 * i;
        top_face.push_back(vertices[v0]);
    }
    // Make sure top_face is in counterclockwise order
    std::reverse(top_face.begin(), top_face.end());
    faces.push_back(top_face);

    // Add bottom face
    std::vector<Vector3D> bottom_face;
    for (size_t i = 0; i < segments; ++i) {
        size_t v1 = 2 * i + 1;
        bottom_face.push_back(vertices[v1]);
    }
    faces.push_back(bottom_face);

    // Add side faces
    for (size_t i = 0; i < segments; ++i) {
        // Indices of the four vertices of the current rectangle
        size_t v0 = 2 * i;
        size_t v1 = v0 + 1;
        size_t v2 = (v0 + 2) % (2 * segments);
        size_t v3 = v2 + 1;

        // Connect vertices to form faces
        std::vector<Vector3D> side_face = { vertices[v0], vertices[v2], 
            vertices[v3], vertices[v1] };

        faces.push_back(side_face);
    }

    return faces;
}


std::vector<std::pair<Vector3D, Vector3D>> pe::returnCylinderEdges(
    const std::vector<Vector3D>& vertices,
    int segments
) {
    std::vector<std::pair<Vector3D, Vector3D>> edges;

    for (size_t i = 0; i < segments; ++i) {
        // Indices of the four vertices of the current rectangle
        size_t v0 = 2 * i;
        size_t v1 = v0 + 1;
        size_t v2 = (v0 + 2) % (2 * segments);
        size_t v3 = v2 + 1;

        // Connect vertices to form edges based on the rectangles
        std::pair<Vector3D, Vector3D> edge1 = { vertices[v0], vertices[v1] };
        std::pair<Vector3D, Vector3D> edge2 = { vertices[v1], vertices[v3] };
        std::pair<Vector3D, Vector3D> edge3 = { vertices[v3], vertices[v2] };
        std::pair<Vector3D, Vector3D> edge4 = { vertices[v2], vertices[v0] };

        edges.push_back(edge1);
        edges.push_back(edge2);
        edges.push_back(edge3);
        edges.push_back(edge4);
    }

    return edges;
}


std::vector<Vector3D> pe::generateConeVertices(const Vector3D& center,
    real radius, real height, int segments) {

    std::vector<Vector3D> vertices;

    // Adds the apex of the cone at the top
    vertices.push_back(center + Vector3D(0.0f, height / 2.0f, 0.0f));

    for (int i = 0; i <= segments; ++i) {
        // Calculate the angle for each segment
        real theta = 2.0f * PI * static_cast<real>(i) / static_cast<real>(segments);

        // Vertices of the current point on the base of the cone
        Vector3D v_base = center + Vector3D(
            radius * cos(theta), 
            -height / 2.0f, 
            radius * sin(theta)
        );

        vertices.push_back(v_base);
    }

    return vertices;
}

std::vector<std::vector<Vector3D>> pe::returnConeFaces(
    const std::vector<Vector3D>& vertices,
    int segments
) {
    std::vector<std::vector<Vector3D>> faces;

    // The first vertex is the apex
    Vector3D apex = vertices[0];

    // Add base face
    std::vector<Vector3D> base_face;
    for (size_t i = 1; i <= segments; ++i) {
        size_t v0 = i;
        base_face.push_back(vertices[v0]);
    }
    faces.push_back(base_face);

    // Add side faces
    for (size_t i = 1; i <= segments; ++i) {
        size_t v0 = i;
        size_t v1 = (i % segments) + 1;

        /*
            Connects vertices to form faces.
        */
        std::vector<Vector3D> side_face = { 
            apex, 
            vertices[v1], 
            vertices[v0]
        };

        faces.push_back(side_face);
    }

    return faces;
}

std::vector<std::pair<Vector3D, Vector3D>> pe::returnConeEdges(
    const std::vector<Vector3D>& vertices,
    int segments
) {
    std::vector<std::pair<Vector3D, Vector3D>> edges;

    for (size_t i = 1; i <= segments; ++i) {
        // Indices of the two vertices of the current edge
        size_t v0 = i;
        size_t v1 = (i % segments) + 1;

        // Connect vertices to form edges based on the side faces
        std::pair<Vector3D, Vector3D> edge = { vertices[v0], vertices[v1] };

        edges.push_back(edge);
    }

    // Add edges from the apex to the base vertices
    for (size_t i = 1; i <= segments; ++i) {
        edges.push_back({ vertices[0], vertices[i] });
    }

    return edges;
}
