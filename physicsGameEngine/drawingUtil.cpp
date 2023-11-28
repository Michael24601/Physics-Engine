
#include "drawingUtil.h"

using namespace pe;


void pe::drawVectorOfLines3D(
    const std::vector<std::pair<Vector3D, Vector3D>>& lines,
    glm::mat4& projection,
    glm::mat4& view,
    glm::mat4& model,
    sf::Color color) {

    glm::mat4 matrix = projection * view * model;

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(glm::value_ptr(matrix));

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    for (const std::pair<Vector3D, Vector3D>& pair : lines) {
        glBegin(GL_LINES);
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