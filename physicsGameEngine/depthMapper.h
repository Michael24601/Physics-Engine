
/*
    This class captures the environment rendered by shaders, from the
    perspective of some source, and returns a texture containing the depth
    of the rendered shapes (a texture where the colors represent the depth
    of the closest object to the source).
    This can be used to map shadows.
*/

#ifndef DEPTH_MAPPER_H
#define DEPTH_MAPPER_H

#include <GL/glew.h>
#include <glm.hpp>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include "simpleShader.h"

namespace pe {

    class DepthMapper {

    private:

        // The resolution of the depth texture 
        int width;
        int height;

        // Texture that will contain the cubemap 
        GLuint depthMap;

        // Framebuffer object
        GLuint framebuffer;

        /*
            Simple shader used to render all objects casting a shadow.
            This shader doesn't output anything in the fragment shader,
            as only the depth value is needed to calculate the shadow/depth
            map.
        */
        SimpleShader shader;

    public:

        DepthMapper(int width, int height) :
            width{ width }, height{ height } {

            glGenFramebuffers(1, &framebuffer);

            glGenTextures(1, &depthMap);
            glBindTexture(GL_TEXTURE_2D, depthMap);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
                width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);

            GLenum framebufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
            if (framebufferStatus != GL_FRAMEBUFFER_COMPLETE) {
                std::cerr << "Framebuffer is incomplete!" << std::endl;
            }

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }


        // Deletes the buffer and texture
        ~DepthMapper() {
            glDeleteFramebuffers(1, &framebuffer);
            glDeleteTextures(1, &depthMap);
        }


        /*
            Captures the environment at the given position, by rendering
            the given shaders.
            The render components are given, but only the vertex buffer
            and model matrices are used; the shader of the given render component
            is set aside in favor of the simple shader.
        */
        void captureDepth(
            const glm::mat4& viewMatrix,
            const glm::mat4& projectionMatrix,
            std::vector<RenderComponent*>& objects
        ) {

            glViewport(0, 0, width, height);
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
            glClear(GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);

            shader.setViewMatrix(viewMatrix);
            shader.setProjectionMatrix(projectionMatrix);

            // Uses each shader to render the scene from this perspective
            for (RenderComponent* object : objects) {
                shader.setModelMatrix(object->model);
                shader.render(*object->vertexBuffer);
            }

            // We finally unbind the framebuffer
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }


        /*
            This function here returns the depth texture.
            (should be used after capturing the depth map).
        */
        GLuint getTexture() const {
            return depthMap;
        }


        int getWidth() const {
            return width;
        }


        int getHeight() const {
            return height;
        }

    };
}

#endif