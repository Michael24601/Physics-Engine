
#include "drawingUtil.h"

using namespace pe;


void pe::drawVectorOfLines3D(
    const std::vector<std::pair<Vector3D, Vector3D>>& lines,
    glm::mat4& projection,
    glm::mat4& view,
    glm::mat4& model,
    sf::Color color,
    real lineWidth) {

    glm::mat4 matrix = projection * view * model;

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(glm::value_ptr(matrix));

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    for (const std::pair<Vector3D, Vector3D>& pair : lines) {
        glBegin(GL_LINES);
        glLineWidth(lineWidth);
        glColor3ub(color.r, color.g, color.b);
        glVertex3f(pair.first.x, pair.first.y, pair.first.z);
        glVertex3f(pair.second.x, pair.second.y, pair.second.z);
        glEnd();
    }
}


void pe::drawVectorOfPolygons3D(
    const std::vector<std::vector<Vector3D>>& polygons,
    glm::mat4& projection,
    glm::mat4& view,
    glm::mat4& model,
    sf::Color color, 
    real opacity) {

    glm::mat4 matrix = projection * view * model;

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(glm::value_ptr(matrix));

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    for (const std::vector<Vector3D>& polygon : polygons) {
        glBegin(GL_POLYGON);
        glColor4ub(color.r, color.g, color.b, opacity);
        for (const Vector3D& point : polygon) {
            glVertex3f(point.x, point.y, point.z);
        }
        glEnd();
    }
}


glm::mat4 pe::convertToGLM(const Matrix3x4& m) {
    return glm::mat4(
        m.data[0], m.data[4], m.data[8], 0.0f,
        m.data[1], m.data[5], m.data[9], 0.0f,
        m.data[2], m.data[6], m.data[10], 0.0f,
        m.data[3], m.data[7], m.data[11], 1.0f
    );
}


/*
    Returns a 3D matrix in the glm format.
*/
glm::vec3 pe::convertToGLM(const Vector3D& v) {
    return glm::vec3(v.x, v.y, v.z);
}


std::vector<std::vector<Vector3D>> pe::triangulateFace(
    const std::vector<Vector3D>& vertices
) {
    std::vector<std::vector<Vector3D>> triangles;

    if (vertices.size() < 3) {
        // Not enough vertices to form a polygon
        return triangles;
    }

    for (size_t i = 1; i < vertices.size() - 1; ++i) {
        triangles.push_back({ vertices[0], vertices[i], vertices[i + 1] });
    }

    return triangles;
}


Vector3D pe::getNormal(const Vector3D& v1, const Vector3D& v2, 
    const Vector3D& v3) {
    // Calculates the local normal using cross product
    Vector3D AB = v2 - v1;
    Vector3D AC = v3 - v1;
    Vector3D normal = AB.vectorProduct(AC);
    normal.normalize();
    return normal;
}


std::vector<Vector3D> pe::generateSphereVertices(const Vector3D& center,
    real radius, int latitudeSegments, int longitudeSegments) {

    std::vector<Vector3D> vertices;

    for (int lat = 0; lat <= latitudeSegments; ++lat) {
        for (int lon = 0; lon <= longitudeSegments; ++lon) {
            // Vertices of the current quad/triangle
            Vector3D v;

            real theta = PI * static_cast<real>(lat)
                / static_cast<real>(latitudeSegments);
            real phi = 2.0f * PI * static_cast<real>(lon)
                / static_cast<real>(longitudeSegments);

            // Calculate the vertex position
            v = center;	// Center
            v += Vector3D(sin(theta) * cos(phi), sin(theta)
                * sin(phi), cos(theta)) * radius;

            // Add the unique vertex to the result vector
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
            std::vector<Vector3D> face1, face2;
            face1 = { vertices[v0] , vertices[v2] ,
                vertices[v1] };
            face2 = { vertices[v1] , vertices[v2] ,
                vertices[v3] };
            faces.push_back(face1);
            faces.push_back(face2);
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
            // Indices of the four vertices of the current quad/triangle
            size_t v0 = lat * (longitudeSegments + 1) + lon;
            size_t v1 = v0 + 1;
            size_t v2 = (lat + 1) * (longitudeSegments + 1) + lon;
            size_t v3 = v2 + 1;


            // Connect vertices to form edges based on the triangles
            std::pair<Vector3D, Vector3D> edge1, edge2, edge3, edge4;
            edge1 = { vertices[v0], vertices[v1] };
            edge2 = { vertices[v1], vertices[v2] };
            edge3 = { vertices[v2], vertices[v3] };
            edge4 = { vertices[v3], vertices[v0] };
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
            // Indices of the four vertices of the current triangle
            // in counterclockwise order.
            size_t v0 = lat * (longitudeSegments + 1) + lon;
            size_t v1 = v0 + 1;
            size_t v2 = (lat + 1) * (longitudeSegments + 1) + lon;
            size_t v3 = v2 + 1;

            // Connect vertices to form faces in counter clockwise order
            std::vector<Vector3D*> face1, face2;
            face1 = { &vertices[v0] , &vertices[v2] ,
                &vertices[v1] };
            face2 = { &vertices[v1] , &vertices[v2] ,
                &vertices[v3] };
            faces.push_back(face1);
            faces.push_back(face2);
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


            // Connect vertices to form edges based on the triangles
            std::pair<Vector3D*, Vector3D*> edge1, edge2, edge3, edge4;
            edge1 = { &vertices[v0], &vertices[v1] };
            edge2 = { &vertices[v1], &vertices[v2] };
            edge3 = { &vertices[v2], &vertices[v3] };
            edge4 = { &vertices[v3], &vertices[v0] };
            edges.push_back(edge1);
            edges.push_back(edge2);
            edges.push_back(edge3);
            edges.push_back(edge4);
        }
    }

    return edges;
}



