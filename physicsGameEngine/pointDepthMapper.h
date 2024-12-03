/*
    Depth mapper for point light projections (perspective light).
    So a cubemap texture is used instead to capture shadows.
*/

#ifndef POINT_DEPTH_MAPPER_H
#define POINT_DEPTH_MAPPER_H

#include <GL/glew.h>
#include <glm.hpp>
#include <array>
#include <vector>
#include "cubemapShader.h"
#include "cubemapGeometryShader.h"

namespace pe {

    class PointDepthMapper {

    private:

        int width, height;
        GLuint depthCubemap;
        GLuint framebuffer;

        /*
            Used in rendering the depth map in 6 passes.
        */
        CubemapShader shader;

        /*
            Used for rendering the map in one pass by making
            use of a geometry shader.
        */
        CubemapGeometryShader shaderOnePass;

    public:

        PointDepthMapper(
            int width, 
            int height
        ) : width(width), height(height) {

            // Cubemap
            glGenTextures(1, &depthCubemap);
            glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);

            for (unsigned int i = 0; i < 6; ++i) {
                glTexImage2D(
                    GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
                    width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr
                );
            }
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

            // Famebuffer
            glGenFramebuffers(1, &framebuffer);
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
            glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);

            GLenum framebufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
            if (framebufferStatus != GL_FRAMEBUFFER_COMPLETE) {
                std::cerr << "Framebuffer is incomplete!" << std::endl;
            }

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        ~PointDepthMapper() {
            glDeleteTextures(1, &depthCubemap);
            glDeleteFramebuffers(1, &framebuffer);
        }

        /*
            We have two options for rendering the depth cubemap.
            We can render the scene 6 times one from each view of
            the cubemap, or we can use a geometry shader that generates
            6 triangles, one from each view.
            This is the first method, and after testing it turned out
            to be faster.
        */
        void captureDepth(
            const glm::vec3& lightPos,
            const std::vector<glm::mat4>& viewMatrices,
            const glm::mat4& projectionMatrix,
            float farPlane,
            std::vector<RenderComponent*>& objects
        ) {

            glViewport(0, 0, width, height);
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

            // 6 passes
            for (size_t i = 0; i < 6; ++i) {
                glFramebufferTexture2D(
                    GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                    GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, depthCubemap, 0
                );
                glClear(GL_DEPTH_BUFFER_BIT);

                // Using the normal shader
                shader.setViewMatrix(viewMatrices[i]);
                shader.setProjectionMatrix(projectionMatrix);
                shader.setFarPlane(farPlane);
                shader.setLightPosition(lightPos);

                for (RenderComponent* object : objects) {
                    shader.setModelMatrix(object->model);
                    shader.render(*object->vertexBuffer);
                }
            }
            

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }


        /*
            This is the second method, and it turned out to be slower.
        */
        void captureDepthOnePass(
            const glm::vec3& lightPos,
            const std::vector<glm::mat4>& viewMatrices,
            const glm::mat4& projectionMatrix,
            float farPlane,
            std::vector<RenderComponent*>& objects
        ) {

            glViewport(0, 0, width, height);
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

            glClear(GL_DEPTH_BUFFER_BIT);

            // Using the shader program with the geometry shader
            shaderOnePass.setViewMatrices(viewMatrices);
            shaderOnePass.setProjectionMatrix(projectionMatrix);
            shaderOnePass.setFarPlane(farPlane);
            shaderOnePass.setLightPosition(lightPos);

            for (RenderComponent* object : objects) {
                shaderOnePass.setModelMatrix(object->model);
                shaderOnePass.render(*object->vertexBuffer);
            }

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }


        GLuint getTexture() const {
            return depthCubemap;
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
