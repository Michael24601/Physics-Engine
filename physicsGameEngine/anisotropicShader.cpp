
#include "anisotropicShader.h"

using namespace pe;

void AnisotropicShader::drawFaces(
    std::vector<glm::vec3>& faces,
    std::vector<glm::vec3>& normals,
    std::vector<glm::vec3>& tangents,
    std::vector<glm::vec3>& bitangents,
    const glm::mat4& model,
    const glm::mat4& view,
    const glm::mat4& projection,
    const glm::vec4& objectColor,
    const glm::vec4& specularColor,
    const glm::vec4& ambientColor,
    const glm::vec3& lightSourcePosition,
    const glm::vec4& lightSourceColor,
    const glm::vec3& viewPosition,
    real alphaX,
    real alphaY
) {

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Calculate the total size of the data
    size_t totalSize = faces.size() * sizeof(glm::vec3)
        + normals.size() * sizeof(glm::vec3)
        + tangents.size() * sizeof(glm::vec3)
        + bitangents.size() * sizeof(glm::vec3);

    // Create a buffer to hold position, normal, tangent, and bitangent data
    std::vector<glm::vec3> combinedData;
    combinedData.reserve(faces.size() + normals.size() + tangents.size());
    combinedData.insert(combinedData.end(), faces.begin(), faces.end());
    combinedData.insert(combinedData.end(), normals.begin(), normals.end());
    combinedData.insert(combinedData.end(), tangents.begin(), tangents.end());
    combinedData.insert(combinedData.end(), bitangents.begin(), bitangents.end());

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
        sizeof(glm::vec3),
        (void*)0
    );

    glEnableVertexAttribArray(0);

    // Specify vertex attribute pointers for normal
    glVertexAttribPointer(
        1, 3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(glm::vec3),
        (void*)((faces.size()) * sizeof(glm::vec3))
    );
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(
        2, 3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(glm::vec3),
        (void*)((faces.size() + normals.size()) * sizeof(glm::vec3))
    );
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(
        3, 3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(glm::vec3),
        (void*)((faces.size() + normals.size() + tangents.size()) * sizeof(glm::vec3))
    );
    glEnableVertexAttribArray(3);

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
    GLint specularColorLoc = glGetUniformLocation(shaderProgram, "specularColor");
    GLint ambientColorLoc = glGetUniformLocation(shaderProgram, "ambientColor");
    GLuint lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos");
    GLuint lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");
    GLuint viewPosLoc = glGetUniformLocation(shaderProgram, "viewPos");
    GLuint alphaXLoc = glGetUniformLocation(shaderProgram, "alphaX");
    GLuint alphaYLoc = glGetUniformLocation(shaderProgram, "alphaY");

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniform4fv(objectColorLoc, 1, &objectColor[0]);
    glUniform4fv(ambientColorLoc, 1, &ambientColor[0]);
    glUniform4fv(specularColorLoc, 1, &specularColor[0]);
    glUniform4fv(lightPosLoc, 1, &lightSourcePosition[0]);
    glUniform4fv(lightColorLoc, 1, &lightSourceColor[0]);
    glUniform3fv(viewPosLoc, 1, &viewPosition[0]);
    glUniform1f(alphaXLoc, alphaX);
    glUniform1f(alphaYLoc, alphaY);

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
