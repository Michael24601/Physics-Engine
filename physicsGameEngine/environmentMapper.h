/*
	This class captures the environment rendered by shaders, and returns
	a cubemap texture containing the environment captured from six different
	directions: up, down, left, right, back, and front.
*/

#ifndef ENVIRONMENT_MAPPER_H
#define ENVIRONMENT_MAPPER_H

#include <GL/glew.h>
#include <glm.hpp>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include "shader.h"

namespace pe {

	class EnvironmentMapper {

	private:

        // The resolution of the cubemap textures 
        float width;
        float height;

		// Texture that will contain the cubemap 
        GLuint cubeMapTexture;
  
        // Framebuffer object
        GLuint framebuffer;

	public:

        EnvironmentMapper(float width, float height) : 
            width{ width }, height{height} {

            // We have a cubemap texture so we use the first index
            glActiveTexture(GL_TEXTURE1);
            glGenTextures(1, &cubeMapTexture);
            glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);

            // Texture parameters
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

            // Storage for each face of the cubemap
            for (GLuint face = 0; face < 6; ++face) {
                glTexImage2D(
                    GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, GL_RGBA,
                    width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr
                );
            }

            glGenFramebuffers(1, &framebuffer);

            /*
                When we are done, we set the active texture back to the one at
                index 0.
            */
            glActiveTexture(GL_TEXTURE0);

		}


        // Deletes the buffer and texture
        ~EnvironmentMapper() {
            glDeleteFramebuffers(1, &framebuffer);
            glDeleteTextures(1, &cubeMapTexture);
        }


        /*
            Captures the environment at the given position, by rendering
            the given shaders.
            We assume all of the uniforms of the shaders were set outside,
            including the projection and model. Only the view matrix of
            the shaders is modified in this function (since we need to use
            a specific view matrix for each of the 6 directions we render
            the scene from).
        */
        void captureEnvironment(
            const glm::vec3& position,
            std::vector<Shader*>& shaders
        ) {

            // The six directions for cubemap faces
            glm::vec3 directions[6] = {
                glm::vec3(1.0f, 0.0f, 0.0f),  // +X
                glm::vec3(-1.0f, 0.0f, 0.0f), // -X
                glm::vec3(0.0f, 1.0f, 0.0f),  // +Y
                glm::vec3(0.0f, -1.0f, 0.0f), // -Y
                glm::vec3(0.0f, 0.0f, 1.0f),  // +Z
                glm::vec3(0.0f, 0.0f, -1.0f)  // -Z
            };

            // Binds the buffer
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

            glActiveTexture(GL_TEXTURE1);

            for (GLuint face = 0; face < 6; face++) {

                // Here we bind the cubemap face as the render target
                glFramebufferTexture2D(
                    GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                    GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, cubeMapTexture, 0
                );

                // Here we set up view matrix for current face
                glm::mat4 view = glm::lookAt(
                    position,
                    position + directions[face],
                    // The up vector is reverse since it is a reflection
                    glm::vec3(0.0f, -1.0f, 0.0f)
                );

                // We have to set the viewport for the current cubemap face
                glViewport(0, 0, width, height);
                glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
                // We then clear the framebuffer
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                // Uses each shader to render the scene from this perspective
                for (Shader* shader : shaders) {
                    shader->setViewMatrix(view);
                    shader->drawFaces();
                }
            }

            // Unbinds the buffer
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            glActiveTexture(GL_TEXTURE0);
        }

        
        /*
            This function here returns the cube map texture
            (should be used after capturing the depth).
        */
        GLuint getTexture() const {
            return cubeMapTexture;
        }

	};
}

#endif