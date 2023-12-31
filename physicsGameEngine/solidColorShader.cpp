
#include "solidColorShader.h"

using namespace pe;

void SolidColorShader::drawEdges(
    const std::vector<glm::vec3>& edges,
    const glm::mat4& model,
    const glm::mat4& view,
    const glm::mat4& projection,
    const glm::vec4& objectColor
) {

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Uploads the vertices data to the VBO
    glBufferData(
        GL_ARRAY_BUFFER, 
        edges.size() * sizeof(glm::vec3), 
        edges.data(), 
        GL_STATIC_DRAW
    );

    // Specifies vertex attribute pointers
    glVertexAttribPointer(
        0, 3, 
        GL_FLOAT, 
        GL_FALSE, 
        sizeof(glm::vec3), 
        (void*)0
    );

    glEnableVertexAttribArray(0);

    // Unbinds VAO
    glBindVertexArray(0);

    // Cleans up the VBO (it is still bound to VAO after unbinding VAO)
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLuint shaderProgram = shaderProgramObject.getShaderProgram();
    glUseProgram(shaderProgram);

    // Sets uniforms
    GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
    GLint objectColorLoc = glGetUniformLocation(shaderProgram, "objectColor");

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniform4fv(objectColorLoc, 1, &objectColor[0]);

    glBindVertexArray(vao);

    // Draws the lines
    glDrawArrays(GL_LINES, 0, edges.size());
    glBindVertexArray(0);

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glUseProgram(0);

    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cout << "OpenGL Error: " << error << '\n';
    }
}

void SolidColorShader::drawFaces(
    const std::vector<glm::vec3>& faces,
    const glm::mat4& model,
    const glm::mat4& view,
    const glm::mat4& projection,
    const glm::vec4& objectColor
) {

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Uploads the vertices data to the VBO
    glBufferData(
        GL_ARRAY_BUFFER, 
        faces.size() * sizeof(glm::vec3),
        faces.data(), GL_STATIC_DRAW
    );

    // Specifies vertex attribute pointers
    glVertexAttribPointer(
        0, 3, 
        GL_FLOAT, GL_FALSE, 
        sizeof(glm::vec3), 
        (void*)0
    );

    glEnableVertexAttribArray(0);

    // Unbinds VAO
    glBindVertexArray(0);

    // Cleans up the VBO (it is still bound to VAO after unbinding VAO)
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLuint shaderProgram = shaderProgramObject.getShaderProgram();
    glUseProgram(shaderProgram);

    // Sets uniforms
    GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
    GLint objectColorLoc = glGetUniformLocation(shaderProgram, "objectColor");

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniform4fv(objectColorLoc, 1, &objectColor[0]);

    glBindVertexArray(vao);

    glDrawArrays(GL_TRIANGLES, 0, faces.size());
    glBindVertexArray(0);

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glUseProgram(0);

    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cout << "OpenGL Error: " << error << '\n';
    }
}