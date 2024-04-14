
#include "drawingUtil.h"

using namespace pe;


void pe::drawVectorOfLines3D(
    const std::vector<std::pair<Vector3D, Vector3D>>& lines,
    glm::mat4& projection,
    glm::mat4& view,
    glm::mat4& model,
    sf::Color color,
    real lineWidth
) {

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
    real opacity
) {

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
