
#include "drawingUtil.h"

using namespace pe;


void pe::drawVectorOfLines3D(
    const std::vector<std::pair<glm::vec3, glm::vec3>>& lines,
    glm::mat4& projection,
    glm::mat4& view,
    glm::mat4& model,
    glm::vec3& color,
    real lineWidth
) {

    glm::mat4 matrix = projection * view * model;

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(glm::value_ptr(matrix));

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    for (const std::pair<glm::vec3, glm::vec3>& pair : lines) {
        glBegin(GL_LINES);
        glLineWidth(lineWidth);
        glColor3ub(color.r, color.g, color.b);
        glVertex3f(pair.first.x, pair.first.y, pair.first.z);
        glVertex3f(pair.second.x, pair.second.y, pair.second.z);
        glEnd();
    }
}


void pe::drawVectorOfPolygons3D(
    const std::vector<std::vector<glm::vec3>>& polygons,
    glm::mat4& projection,
    glm::mat4& view,
    glm::mat4& model,
    glm::vec3& color, 
    real opacity
) {

    glm::mat4 matrix = projection * view * model;

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(glm::value_ptr(matrix));

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    for (const std::vector<glm::vec3>& polygon : polygons) {
        glBegin(GL_POLYGON);
        glColor4ub(color.r, color.g, color.b, opacity);
        for (const glm::vec3& point : polygon) {
            glVertex3f(point.x, point.y, point.z);
        }
        glEnd();
    }
}
