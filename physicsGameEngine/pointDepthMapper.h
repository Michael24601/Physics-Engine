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
#include "simpleShader.h"

namespace pe {

    class PointDepthMapper {

    private:

        int width, height;
        GLuint depthCubemap;
        GLuint framebuffer;

        SimpleShader shader;

        float nearPlane;
        float farPlane;
        float fov;

        // Six view matrices are generated for the cubemap
        std::array<glm::mat4, 6> getViewMatrices(const glm::vec3& lightPos) const {
            return {
                glm::lookAt(lightPos, lightPos + glm::vec3(1, 0, 0), glm::vec3(0, -1, 0)), // +X
                glm::lookAt(lightPos, lightPos + glm::vec3(-1, 0, 0), glm::vec3(0, -1, 0)), // -X
                glm::lookAt(lightPos, lightPos + glm::vec3(0, 1, 0), glm::vec3(0, 0, 1)),   // +Y
                glm::lookAt(lightPos, lightPos + glm::vec3(0, -1, 0), glm::vec3(0, 0, -1)), // -Y
                glm::lookAt(lightPos, lightPos + glm::vec3(0, 0, 1), glm::vec3(0, -1, 0)),  // +Z
                glm::lookAt(lightPos, lightPos + glm::vec3(0, 0, -1), glm::vec3(0, -1, 0))  // -Z
            };
        }

    public:

        PointDepthMapper(
            int width, 
            int height, 
            float nearPlane, 
            float farPlane, 
            float fov = 90.0f
        ) : width(width), height(height), nearPlane(nearPlane), farPlane(farPlane), fov(fov) {

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

        // Renders the depth map from a point light
        void captureDepth(
            const glm::vec3& lightPos,
            std::vector<RenderComponent*>& objects
        ) {

            float aspect = (float)width / height;
            glm::mat4 projection = glm::perspective(
                glm::radians(fov), 
                aspect,
                nearPlane, 
                farPlane
            );

            auto viewMatrices = getViewMatrices(lightPos);

            glViewport(0, 0, width, height);
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
            glClear(GL_DEPTH_BUFFER_BIT);

            for (size_t i = 0; i < 6; ++i) {
                glFramebufferTexture2D(
                    GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                    GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, depthCubemap, 0
                );
                glClear(GL_DEPTH_BUFFER_BIT);

                shader.setViewMatrix(viewMatrices[i]);
                shader.setProjectionMatrix(projection);

                for (RenderComponent* object : objects) {
                    shader.setModelMatrix(object->model);
                    shader.render(*object->vertexBuffer);
                }
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

        float getNearPlane() const {
            return nearPlane;
        }

        float getFarPlane() const {
            return farPlane;
        }
    };

}

#endif
