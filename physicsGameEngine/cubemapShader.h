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

#ifndef CUBEMAP_SHADER_H
#define CUBEMAP_SHADER_H

#include "shader.h"
#include "vertexBuffer.h"

namespace pe {

    class CubemapShader : public Shader{

    private:
        
        VertexBuffer buffer;

    public:

        CubemapShader() : Shader(
            "cubemapShader.vert.glsl",
            "cubemapShader.frag.glsl",
            std::vector<unsigned int>{3}
        ) {}

        void setSkybox(GLuint skyboxTextureId) {
            setTextureUniform(
                "skybox",
                skyboxTextureId,
                GL_TEXTURE_CUBE_MAP,
                0
            );
        }


        void setScale(float scale) {
            setUniform("scale", scale);
        }


        void setViewMatrix(const glm::mat4& view) {
            setUniform("view", view);
        }


        void setProjectionMatrix(const glm::mat4& projection) {
            setUniform("projection", projection);
        }


        void setDarknessLevel(float darkness) {
            setUniform("darkness", darkness);
        }


        /*
            Since this shader will be used to draw a skybox, we have
            to override the render function so that we can disable depth
            writing before we start drawing. This ensures the skybox,
            used for drawing the background, appears in the back of all
            objects.

            Ensure that the sent vertex buffer is actually the vertex
            buffer of a skybox, which is essentially the vertices of a
            box, but with the vertices in reverse order so that the
            faces of the box are drawn on the inside, not outside.
        */
        void render(const VertexBuffer& buffer) override {
       
            // First we disable depth writing
            glDepthMask(GL_FALSE);

            // Then we call the normal draw function
            Shader::render(buffer);

            // Then we re-enable it
            glDepthMask(GL_TRUE);
        }
    };
}

#endif