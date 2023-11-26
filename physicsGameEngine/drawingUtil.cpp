
#include "drawingUtil.h"

using namespace pe;


void pe::drawVectorOfLines(
    const std::vector<std::pair<Vector3D, Vector3D>>& vector,
    sf::RenderWindow& window,
    sf::Color color) {

    for (const std::pair<Vector3D, Vector3D>& pair : vector) {
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


void pe::drawVectorOfPolygons3D(
    const std::vector<std::vector<Vector3D>>& polygons,
    glm::mat4& viewProjectionMatrix,
    sf::Color color, 
    real opacity) {

    // We need to manually use the projection matrix as we don't have a
    // vertex shader.
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(glm::value_ptr(viewProjectionMatrix));

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


GLuint createVAO(const std::vector<Vector3D>& flattenedData) {
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Upload cube data to VBO
    glBufferData(GL_ARRAY_BUFFER, flattenedData.size() * sizeof(Vector3D),
        flattenedData.data(), GL_STATIC_DRAW);

    // Specify vertex attribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3D), (void*)0);
    glEnableVertexAttribArray(0);

    // Unbind VAO to prevent accidental modification
    glBindVertexArray(0);

    // Cleanup VBO (it is still bound to VAO after unbinding VAO)
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return vao;
}


// Works because all vertices are ordered (clockwise or counter clockwise).
// Assumes the points are in order and convex.
std::vector<std::vector<Vector3D>> triangulateFace(
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


void pe::drawVectorOfPolygons3D(
    const std::vector<std::vector<Vector3D>>& polygons,
    glm::mat4& viewProjectionMatrix,
    GLuint& shaderProgram,
    real r, real g, real b) {

    // Flatten the cube data and triangulate the faces
    std::vector<Vector3D> flattenedData;
    for (const auto& face : polygons) {
        auto triangles = triangulateFace(face);
        flattenedData.reserve(triangles.size() * 3);
        for (const auto& triangle : triangles) {
            flattenedData.insert(flattenedData.end(), triangle.begin(), triangle.end());
        }
    }

    // Don't declare as static, since that will just make it retain
    // position, and objects won't move or be colored correctly.
    GLuint vao = createVAO(flattenedData);

    glm::vec3 objectColor(
        static_cast<real>(r) / 255.0f,
        static_cast<real>(g) / 255.0f,
        static_cast<real>(b) / 255.0f
    );

    GLint viewProjectionLoc = glGetUniformLocation(shaderProgram, "viewProjection");
    GLint objectColorLoc = glGetUniformLocation(shaderProgram, "objectColor");

    glUseProgram(shaderProgram);

    glUniformMatrix4fv(viewProjectionLoc, 1, GL_FALSE, glm::value_ptr(viewProjectionMatrix));
    glUniform3fv(objectColorLoc, 1, &objectColor[0]);

    glBindVertexArray(vao);

    glDrawArrays(GL_TRIANGLES, 0, flattenedData.size());
    glBindVertexArray(0);

    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cout << "OpenGL Error: " << error << '\n';
    }
}


void pe::drawVectorOfLines3D(
    const std::vector<std::pair<Vector3D, Vector3D>>& lines,
    glm::mat4& viewProjectionMatrix,
    GLuint shaderProgram,
    real r, real g, real b
) {
    glUseProgram(shaderProgram);

    GLint viewProjectionLoc = glGetUniformLocation(shaderProgram, "viewProjection");
    GLint objectColorLoc = glGetUniformLocation(shaderProgram, "objectColor");

    glm::vec3 objectColor(
        static_cast<real>(r) / 255.0f,
        static_cast<real>(g) / 255.0f,
        static_cast<real>(b) / 255.0f
    );

    glUniformMatrix4fv(viewProjectionLoc, 1, GL_FALSE, glm::value_ptr(viewProjectionMatrix));
    glUniform3fv(objectColorLoc, 1, &objectColor[0]);

    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    std::vector<float> vertices;
    for (const auto& pair : lines) {
        vertices.push_back(pair.first.x);
        vertices.push_back(pair.first.y);
        vertices.push_back(pair.first.z);

        vertices.push_back(pair.second.x);
        vertices.push_back(pair.second.y);
        vertices.push_back(pair.second.z);
    }

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // Specify vertex attribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Unbind VAO to prevent accidental modification
    glBindVertexArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Draw lines
    glBindVertexArray(vao);
    glDrawArrays(GL_LINES, 0, vertices.size() / 3);
    glBindVertexArray(0);

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);

    glUseProgram(0);
}