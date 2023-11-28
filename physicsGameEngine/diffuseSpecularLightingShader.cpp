
#include "diffuseSpecularLightingShader.h"

using namespace pe;

void DiffuseSpecularLightingShader::drawFaces(
    const std::vector<std::vector<Vector3D>>& faces,
    const glm::mat4& model,
    const glm::mat4& view,
    const glm::mat4& projection,
    const glm::vec4& objectColor,
    int activeLightSources,
    glm::vec3* lightSourcesPosition,
    glm::vec4* lightSourcesColor,
    const glm::vec3& viewPosition,
    real shininess
) {

    if (activeLightSources > MAXIMUM_NUMBER_OF_LIGHT_SOURCES) {
        std::cerr << "At most, " << MAXIMUM_NUMBER_OF_LIGHT_SOURCES
            << " light sources can be rendered";
        return;
    }

    // First we flatten the polyhedra data and triangulate the faces
    // And extract the normals
    std::vector<Vector3D> flattenedPositions;
    std::vector<Vector3D> flattenedNormals;

    // Flatten the cube data and triangulate the faces
    for (const auto& face : faces) {
        auto triangles = triangulateFace(face);
        flattenedPositions.reserve(triangles.size() * 3);
        flattenedNormals.reserve(triangles.size() * 3);
        for (const auto& triangle : triangles) {
            flattenedPositions.insert(flattenedPositions.end(),
                triangle.begin(), triangle.end());
            // Calculate the normal for the triangle and replicate it for each vertex
            Vector3D faceNormal = getNormal(triangle[0], triangle[1], triangle[2]);
            flattenedNormals.push_back(faceNormal);
            flattenedNormals.push_back(faceNormal);
            flattenedNormals.push_back(faceNormal);
        }
    }

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Calculate the total size of the data
    size_t totalSize = flattenedPositions.size() * sizeof(Vector3D)
        + flattenedNormals.size() * sizeof(Vector3D);

    // Create a buffer to hold both position and normal data
    std::vector<Vector3D> combinedData;
    combinedData.reserve(flattenedPositions.size()
        + flattenedNormals.size());
    combinedData.insert(combinedData.end(),
        flattenedPositions.begin(), flattenedPositions.end());
    combinedData.insert(combinedData.end(),
        flattenedNormals.begin(), flattenedNormals.end());

    // Upload combined data to VBO
    glBufferData(
        GL_ARRAY_BUFFER,
        totalSize,
        combinedData.data(),
        GL_STATIC_DRAW
    );

    // Specify vertex attribute pointers for position
    glVertexAttribPointer(
        0, 3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vector3D),
        (void*)0
    );

    glEnableVertexAttribArray(0);

    // Specify vertex attribute pointers for normal
    glVertexAttribPointer(
        1, 3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vector3D),
        (void*)(flattenedPositions.size() * sizeof(Vector3D))
    );
    // Here, 1, is the (location = 1) in the "in" of the shader
    glEnableVertexAttribArray(1);

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
    GLuint lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos");
    GLuint lightColorsLoc = glGetUniformLocation(shaderProgram, "lightColors");
    GLuint numActiveLightsLoc = glGetUniformLocation(shaderProgram, "numActiveLights");
    GLuint viewPosLoc = glGetUniformLocation(shaderProgram, "viewPos");
    GLuint shininessLoc = glGetUniformLocation(shaderProgram, "shininess");

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniform4fv(objectColorLoc, 1, &objectColor[0]);
    glUniform3fv(lightPosLoc, activeLightSources, &lightSourcesPosition[0][0]);
    glUniform4fv(lightColorsLoc, activeLightSources, &lightSourcesColor[0][0]);
    glUniform1i(numActiveLightsLoc, activeLightSources);
    glUniform3fv(viewPosLoc, 1, &viewPosition[0]);
    glUniform1f(shininessLoc, shininess);

    glBindVertexArray(vao);

    glDrawArrays(GL_TRIANGLES, 0, flattenedPositions.size());
    glBindVertexArray(0);

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glUseProgram(0);

    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cout << "OpenGL Error: " << error << '\n';
    }
}