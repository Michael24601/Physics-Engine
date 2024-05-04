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
#include "shader.h"

namespace pe {

    class DepthMapper {

    private:

        // The resolution of the depth texture 
        float width;
        float height;

        // Texture that will contain the cubemap 
        GLuint depthMap;

        // Framebuffer object
        GLuint framebuffer;

    public:

        DepthMapper(float width, float height) :
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
            We assume all of the uniforms of the shaders were set outside,
            including the model. Only the view matrix and projection
            matrices of the shaders are modified in this function (since
            we need to use a specific view matrix for the source we are
            given, and the projection may change depending on the type
            of lighting).

            Any shader can be used, but it is recommneded to use a simple
            shader that doesn't write any colors in the fragment shader.
            This is because we only care for the fragment depth.
        */
        void captureDepth(
            const glm::mat4& viewMatrix,
            const glm::mat4& projectionMatrix,
            std::vector<Shader*>& shaders
        ) {
            glViewport(0, 0, width, height);
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
            glClear(GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);

            // Uses each shader to render the scene from this perspective
            for (Shader* shader : shaders) {
                shader->setViewMatrix(viewMatrix);
                shader->setProjectionMatrix(projectionMatrix);
                shader->drawFaces();
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

    };
}

#endif