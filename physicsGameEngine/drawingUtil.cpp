
#include "drawingUtil.h"

using namespace pe;


void pe::drawVectorOfLines(
    const std::vector<std::pair<Vector3D, Vector3D>>& vector,
    sf::RenderWindow& window,
    sf::Color color) {

    for (const std::pair<Vector3D, Vector3D> pair : vector) {
        sf::VertexArray line(sf::LinesStrip, 2);
        line[0] = sf::Vector2f(pair.first.x, pair.first.y);
        line[1] = sf::Vector2f(pair.second.x, pair.second.y);
        line[0].color = line[1].color = color;
        window.draw(line);
    }
}


void pe::drawVectorOfLines3D(
    const std::vector<std::pair<Vector3D, Vector3D>>& lines,
    glm::mat4& viewProjectionMatrix,
    sf::RenderWindow& window,
    sf::Color color) {

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(glm::value_ptr(viewProjectionMatrix));

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