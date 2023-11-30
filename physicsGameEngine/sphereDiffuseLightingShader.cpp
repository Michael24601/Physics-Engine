
#include "sphereDiffuseLightingShader.h"

using namespace pe;

void SphereDiffuseLightingShader::drawFace(
    const std::vector<std::vector<Vector3D>>& faces,
    const glm::vec3& centerPosition,
    real radius,
    const glm::mat4& model,
    const glm::mat4& view,
    const glm::mat4& projection,
    const glm::vec4& objectColor,
    int activeLightSources,
    glm::vec3* lightSourcesPosition,
    glm::vec4* lightSourcesColor
) {

    std::vector<Vector3D> flattenedData;
    for (const auto& face : faces) {
        auto triangles = triangulateFace(face);
        flattenedData.reserve(triangles.size() * 3);
        for (const auto& triangle : triangles) {
            flattenedData.insert(flattenedData.end(),
                triangle.begin(), triangle.end());
        }
    }

    if (activeLightSources > MAXIMUM_NUMBER_OF_LIGHT_SOURCES) {
        std::cerr << "At most, " << MAXIMUM_NUMBER_OF_LIGHT_SOURCES
            << " light sources can be rendered";
        return;
    }

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Uploads the vertices data to the VBO
    glBufferData(
        GL_ARRAY_BUFFER,
        flattenedData.size() * sizeof(Vector3D),
        flattenedData.data(), GL_STATIC_DRAW
    );

    // Specifies vertex attribute pointers
    glVertexAttribPointer(
        0, 3,
        GL_FLOAT, GL_FALSE,
        sizeof(Vector3D),
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
    GLint centerLoc = glGetUniformLocation(shaderProgram, "sphereCenter");
    GLint radiusLoc = glGetUniformLocation(shaderProgram, "sphereRadius");
    GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
    GLint objectColorLoc = glGetUniformLocation(shaderProgram, "objectColor");
    GLuint lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos");
    GLuint lightColorsLoc = glGetUniformLocation(shaderProgram, "lightColors");
    GLuint numActiveLightsLoc = glGetUniformLocation(shaderProgram, "numActiveLights");

    glUniform3fv(centerLoc, 1, &centerPosition[0]);
    glUniform1f(radiusLoc, radius);
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniform4fv(objectColorLoc, 1, &objectColor[0]);
    glUniform3fv(lightPosLoc, activeLightSources, &lightSourcesPosition[0][0]);
    glUniform4fv(lightColorsLoc, activeLightSources, &lightSourcesColor[0][0]);
    glUniform1i(numActiveLightsLoc, activeLightSources);

    glBindVertexArray(vao);

    glDrawArrays(GL_TRIANGLES, 0, flattenedData.size());
    glBindVertexArray(0);

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glUseProgram(0);

    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cout << "OpenGL Error: " << error << '\n';
    }
}