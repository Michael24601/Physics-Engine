
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


// Also works because vectors are in the same order
Vector3D getNormal(const Vector3D& v1, const Vector3D& v2, 
    const Vector3D& v3) {
    // Calculate the local normal using cross product
    Vector3D AB = v2 - v1;
    Vector3D AC = v3 - v1;
    Vector3D normal = AB.vectorProduct(AC);
    normal.normalize();
    return normal;
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


GLuint createVAO(const std::vector<Vector3D>& positions, const std::vector<Vector3D>& normals) {
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Calculate the total size of the data
    size_t totalSize = positions.size() * sizeof(Vector3D) + normals.size() * sizeof(Vector3D);

    // Create a buffer to hold both position and normal data
    std::vector<Vector3D> combinedData;
    combinedData.reserve(positions.size() + normals.size());
    combinedData.insert(combinedData.end(), positions.begin(), positions.end());
    combinedData.insert(combinedData.end(), normals.begin(), normals.end());

    // Upload combined data to VBO
    glBufferData(GL_ARRAY_BUFFER, totalSize, combinedData.data(), GL_STATIC_DRAW);

    // Specify vertex attribute pointers for position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3D), (void*)0);
    glEnableVertexAttribArray(0);

    // Specify vertex attribute pointers for normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3D), 
        (void*)(positions.size() * sizeof(Vector3D)));
    // Here, 1, is the (location = 1) in the "in" of the shader
    glEnableVertexAttribArray(1);

    // Unbind VAO to prevent accidental modification
    glBindVertexArray(0);
    // Cleanup VBO (it is still bound to VAO after unbinding VAO)
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return vao;
}



void pe::drawVectorOfPolygons3D(
    const std::vector<std::vector<Vector3D>>& polygons,
    glm::mat4& projection,
    glm::mat4& view,
    glm::mat4& model,
    GLuint& shaderProgram,
    real r, real g, real b, real a) {

    std::vector<Vector3D> flattenedPositions;
    std::vector<Vector3D> flattenedNormals;

    // Flatten the cube data and triangulate the faces
    for (const auto& face : polygons) {
        auto triangles = triangulateFace(face);
        flattenedPositions.reserve(triangles.size() * 3);
        flattenedNormals.reserve(triangles.size() * 3);
        for (const auto& triangle : triangles) {
            flattenedPositions.insert(flattenedPositions.end(), triangle.begin(), triangle.end());
            // Calculate the normal for the triangle and replicate it for each vertex
            Vector3D faceNormal = getNormal(triangle[0], triangle[1], triangle[2]);
            flattenedNormals.push_back(faceNormal);
            flattenedNormals.push_back(faceNormal);
            flattenedNormals.push_back(faceNormal);
        }
    }

    // Don't declare as static, since that will just make it retain
    // position, and objects won't move or be colored correctly.
    GLuint vao = createVAO(flattenedPositions, flattenedNormals);

    glm::vec4 objectColor(
        static_cast<real>(r) / 255.0f,
        static_cast<real>(g) / 255.0f,
        static_cast<real>(b) / 255.0f,
        static_cast<real>(a) / 255.0f
    );

    GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLint objectColorLoc = glGetUniformLocation(shaderProgram, "objectColor");
    GLint lightPosLoc1 = glGetUniformLocation(shaderProgram, "lightPos1");
    GLint lightPosLoc2 = glGetUniformLocation(shaderProgram, "lightPos2");
    GLint lightPosLoc3 = glGetUniformLocation(shaderProgram, "lightPos3");

    glUseProgram(shaderProgram);

    // Set the view-projection matrix uniform
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniform4fv(objectColorLoc, 1, &objectColor[0]);
    // Set the light position uniform
    glm::vec3 lightPos1(0.0f, 300.0f, 0.0f);  // Set your light position
    glm::vec3 lightPos2(200.0f, -200.0f, -200.0f);  // Set your light position
    glm::vec3 lightPos3(-200.0f, -200.0f, 200.0f);  // Set your light position
    glUniform3fv(lightPosLoc1, 1, glm::value_ptr(lightPos1));
    glUniform3fv(lightPosLoc2, 1, glm::value_ptr(lightPos2));    
    glUniform3fv(lightPosLoc3, 1, glm::value_ptr(lightPos3));

    glBindVertexArray(vao);

    glDrawArrays(GL_TRIANGLES, 0, flattenedPositions.size());
    glBindVertexArray(0);

    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cout << "OpenGL Error: " << error << '\n';
    }
}


void pe::drawVectorOfLines3D(
    const std::vector<std::pair<Vector3D, Vector3D>>& lines,
    glm::mat4& projection,
    glm::mat4& view,
    glm::mat4& model,
    GLuint shaderProgram,
    real r, real g, real b, real a
) {
    
    glm::vec4 objectColor(
        static_cast<real>(r) / 255.0f,
        static_cast<real>(g) / 255.0f,
        static_cast<real>(b) / 255.0f,
        static_cast<real>(a) / 255.0f
    );

    GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLint objectColorLoc = glGetUniformLocation(shaderProgram, "objectColor");
    GLint lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos");

    glUseProgram(shaderProgram);

    // Set the view-projection matrix uniform
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniform4fv(objectColorLoc, 1, &objectColor[0]);
    // Set the light position uniform
    glm::vec3 lightPos(1.0f, 1.0f, 2.0f);  // Set your light position
    glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));

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