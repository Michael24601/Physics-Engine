
#include "skyboxShader.h"

using namespace pe;

void SkyboxShader::drawSkybox(
    const glm::mat4& view,
    const glm::mat4& projection,
    const GLuint& cubemap,
    real size
) {
    /*
        First we disable depth writing so that the skybox is rendered
        behind everything else.
    */
    glDepthMask(GL_FALSE);

    GLuint shaderProgram = shaderProgramObject.getShaderProgram();
    glUseProgram(shaderProgram);

    /*
        The vertex positions sent to the skybox shader are always those of
        a unit cube(6 faces, 2 triangles per face, 3 vertices per triangle).
        They are given in clockwise order in order to be rendered from the 
        inside, not outside of the cube, since the scene is meant to be
        inside the cubemap.
    */
    GLfloat skyboxVertices[] = {       
        // Front face
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        // Back face
        1.0f,  1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,
       -1.0f, -1.0f, 1.0f,
       -1.0f, -1.0f, 1.0f,
       -1.0f,  1.0f, 1.0f,
        1.0f,  1.0f, 1.0f,

        // Left face
       -1.0f,  1.0f,  1.0f,
       -1.0f, -1.0f,  1.0f,
       -1.0f, -1.0f, -1.0f,
       -1.0f, -1.0f, -1.0f,
       -1.0f,  1.0f, -1.0f,
       -1.0f,  1.0f,  1.0f,

       // Right face
       1.0f,  1.0f, -1.0f,
       1.0f, -1.0f, -1.0f,
       1.0f, -1.0f,  1.0f,
       1.0f, -1.0f,  1.0f,
       1.0f,  1.0f,  1.0f,
       1.0f,  1.0f, -1.0f,

       // Top face
      -1.0f,  1.0f,  1.0f,
      -1.0f,  1.0f, -1.0f,
       1.0f,  1.0f, -1.0f,
       1.0f,  1.0f, -1.0f,
       1.0f,  1.0f,  1.0f,
      -1.0f,  1.0f,  1.0f,

      // Bottom face
     -1.0f, -1.0f, -1.0f,
     -1.0f, -1.0f,  1.0f,
      1.0f, -1.0f,  1.0f,
      1.0f, -1.0f,  1.0f,
      1.0f, -1.0f, -1.0f,
     -1.0f, -1.0f, -1.0f
    };

    GLuint skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);

    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices[0], GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // The identity matrix
    glm::mat4 model = glm::mat4(1.0f);
    // Used to scale the skybox to a larger size
    model = glm::scale(model, glm::vec3(size));

    GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLint projectionLoc = glGetUniformLocation(shaderProgram, "projection");

    // Set view and projection matrices
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);

    // We then render the skybox cube
    glBindVertexArray(skyboxVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    // At the end, we reenable depth writing
    glDepthMask(GL_TRUE);
}