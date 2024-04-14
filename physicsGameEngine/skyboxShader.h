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

namespace pe {

    class SkyboxShader : public Shader{

    public:

        /*
            The never changing vertices(in unit coordinates, though they
            can be scaled).
        */
        const static GLfloat skyboxVertices[]{
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

        SkyboxShader() : Shader(
            "skyboxVertexShader.glsl",
            "skyboxFragmentShader.glsl"
        ) {}

        /*
            Instead of sending an entire model matrix, we only send the 
            scaling factor that will resize the cube to fit our scene
            (since we don't need to translate and rotate it).
        */
        void setModelMatrix(float scale) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::scale(model, glm::vec3(scale));
            setUniform("model", model);
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
        void drawFaces() override {
       
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