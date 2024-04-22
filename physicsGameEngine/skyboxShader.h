/*
    This is the header file for a class representing a special sort of
    shader. Unlike other shaders, this shader is not used for drawing
    shapes, but for drawing the skybox.
    The skybox is like a 3D version of a background; it can be simulated
    by rendering the 6 faces of a cube from the inside, then scaling the
    cube such that our entire scene fits inside it.
    In order to render the cube from the inside, we send the triangles
    representing the cube faces in clockwise order, the opposite of
    counter-clockwise which is the expected order in this engine. As
    such, the VAO and VBOs are not set according to any shapes sent by
    the user, but always remain the same; the only input the user has is
    the size of the cube, which must be large enough to fit the scene.
    This can be expressed using the model matrix, which on top of 
    rotating and translating shapes, can scale them.
*/

#ifndef SKYBOX_SHADER_H
#define SKYBOX_SHADER_H

#include "shader.h"

/*
    The never changing vertices(in unit coordinates, though they can be 
    scaled).
*/
const static std::vector<glm::vec3> skyboxVertices {
    // Front face
    glm::vec3(-1.0f,  1.0f, -1.0f),
    glm::vec3(-1.0f, -1.0f, -1.0f),
    glm::vec3(1.0f, -1.0f, -1.0f),
    glm::vec3(1.0f, -1.0f, -1.0f),
    glm::vec3(1.0f,  1.0f, -1.0f),
    glm::vec3(-1.0f,  1.0f, -1.0f),

    // Back face
    glm::vec3(1.0f,  1.0f,  1.0f),
    glm::vec3(1.0f, -1.0f,  1.0f),
    glm::vec3(-1.0f, -1.0f,  1.0f),
    glm::vec3(-1.0f, -1.0f,  1.0f),
    glm::vec3(-1.0f,  1.0f,  1.0f),
    glm::vec3(1.0f,  1.0f,  1.0f),

    // Left face
    glm::vec3(-1.0f,  1.0f,  1.0f),
    glm::vec3(-1.0f, -1.0f,  1.0f),
    glm::vec3(-1.0f, -1.0f, -1.0f),
    glm::vec3(-1.0f, -1.0f, -1.0f),
    glm::vec3(-1.0f,  1.0f, -1.0f),
    glm::vec3(-1.0f,  1.0f,  1.0f),

    // Right face
    glm::vec3(1.0f,  1.0f, -1.0f),
    glm::vec3(1.0f, -1.0f, -1.0f),
    glm::vec3(1.0f, -1.0f,  1.0f),
    glm::vec3(1.0f, -1.0f,  1.0f),
    glm::vec3(1.0f,  1.0f,  1.0f),
    glm::vec3(1.0f,  1.0f, -1.0f),

    // Top face
    glm::vec3(-1.0f,  1.0f,  1.0f),
    glm::vec3(-1.0f,  1.0f, -1.0f),
    glm::vec3(1.0f,  1.0f, -1.0f),
    glm::vec3(1.0f,  1.0f, -1.0f),
    glm::vec3(1.0f,  1.0f,  1.0f),
    glm::vec3(-1.0f,  1.0f,  1.0f),

    // Bottom face
    glm::vec3(-1.0f, -1.0f, -1.0f),
    glm::vec3(-1.0f, -1.0f,  1.0f),
    glm::vec3(1.0f, -1.0f,  1.0f),
    glm::vec3(1.0f, -1.0f,  1.0f),
    glm::vec3(1.0f, -1.0f, -1.0f),
    glm::vec3(-1.0f, -1.0f, -1.0f)
};

namespace pe {

    class SkyboxShader : public Shader{

    public:

        SkyboxShader() : Shader(
            "skyboxVertexShader.glsl",
            "skyboxFragmentShader.glsl"
        ) {
            /*
                Because the skybox vertices are always those of the cube,
                we can set the VAOand VBO in the constructor automatically.
            */
            std::vector<std::vector<glm::vec3>> data{ skyboxVertices };
            sendVaribleData(data, GL_STATIC_DRAW);
            setTrianglesNumber(skyboxVertices.size());
        }

        void setSkybox(GLuint skyboxTextureId) {
            setTextureUniform(
                "skybox",
                skyboxTextureId,
                GL_TEXTURE_CUBE_MAP,
                0
            );
        }

        /*
            While we can set the model matrix manually when rendering
            a skybox (since the uniform exists in the shader), we usually
            only really want to scale the cube to fit our scene
            (since we don't need to translate and rotate it).
            So we can use this function instead which takes in a scale
            and applies it to the identity matrix, then calls
            setModel.
        */
        void setModelScale(float scale) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::scale(model, glm::vec3(scale));
            setModelMatrix(model);
        }

        void setViewMatrix(const glm::mat4& view) {
            setUniform("view", view);
        }

        void setProjectionMatrix(const glm::mat4& projection) {
            setUniform("projection", projection);
        }

        /*
            When we have a shader responsible for drawing a skybox, we
            can't just use the normal draw function.
            Instead, we have to override it, so that we can disable depth
            writing before we start drawing. This ensures the skybox,
            used for drawing the background, appears in the back of all
            objects.
        */
        void drawSkyboxFaces() {
       
            // First we disable depth writing
            glDepthMask(GL_FALSE);

            // Then we call the normal draw function
            Shader::drawFaces();

            // Then we re-enable it
            glDepthMask(GL_TRUE);
        }
    };
}

#endif