
#include "anisotropicShader.h"

using namespace pe;

void AnisotropicShader::drawFaces(
    const std::vector<glm::vec3>& faces,
    const std::vector<glm::vec3>& normals,
    const std::vector<glm::vec3>& tangents,
    const std::vector<glm::vec3>& bitangents,
    const glm::mat4& model,
    const glm::mat4& view,
    const glm::mat4& projection,
    const glm::vec4& objectColor,
    int activeLightSources,
    glm::vec3* lightSourcesPosition,
    glm::vec4* lightSourcesColor,
    const glm::vec3& viewPosition,
    float roughness,
    float fresnel
) {
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

    // Calculate the total size of the data
    size_t totalSize = faces.size() * sizeof(glm::vec3)
        + normals.size() * sizeof(glm::vec3)
        + tangents.size() * sizeof(glm::vec3)
        + bitangents.size() * sizeof(glm::vec3);

    // Create a buffer to hold position, normal, tangent, and bitangent data
    std::vector<glm::vec3> combinedData;
    combinedData.reserve(faces.size() + normals.size() + tangents.size() + bitangents.size());
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

    // Specify vertex attribute pointers for tangent
    glVertexAttribPointer(
        2, 3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(glm::vec3),
        (void*)((faces.size() + normals.size()) * sizeof(glm::vec3))
    );
    glEnableVertexAttribArray(2);

    // Specify vertex attribute pointers for bitangent
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
    GLuint lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos");
    GLuint lightColorsLoc = glGetUniformLocation(shaderProgram, "lightColors");
    GLuint numActiveLightsLoc = glGetUniformLocation(shaderProgram, "numActiveLights");
    GLuint viewPosLoc = glGetUniformLocation(shaderProgram, "viewPos");
    GLuint roughnessLoc = glGetUniformLocation(shaderProgram, "roughness");
    GLuint fresnelLoc = glGetUniformLocation(shaderProgram, "fresnel");

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniform4fv(objectColorLoc, 1, &objectColor[0]);
    glUniform3fv(lightPosLoc, activeLightSources, &lightSourcesPosition[0][0]);
    glUniform4fv(lightColorsLoc, activeLightSources, &lightSourcesColor[0][0]);
    glUniform1i(numActiveLightsLoc, activeLightSources);
    glUniform3fv(viewPosLoc, 1, &viewPosition[0]);
    glUniform1f(roughnessLoc, roughness);
    glUniform1f(fresnelLoc, fresnel);

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
