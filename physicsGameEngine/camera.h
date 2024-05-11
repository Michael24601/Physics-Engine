
#ifndef CAMERA_H
#define CAMERA_H

#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include "accuracy.h"

namespace pe {

	class Camera {

    protected:

        GLFWwindow* window;

        // Position of the camera
        glm::vec3 cameraPosition;
        // Point the camera is looking at
        glm::vec3 cameraTarget;
        // The up vector indicating the basis we are in
        glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);

        // The view matrix of the camera, indicating where it is looking at
        glm::mat4 viewMatrix;

        // Field of View (FOV) in degrees
        float fov;
        // Aspect ratio
        float aspectRatio;
        // Near and far clipping planes
        float nearPlane;
        float farPlane;

        // The projection matrix is used for perspective
        glm::mat4 projectionMatrix;


        static inline glm::mat4 calculateViewMatrix(
            const glm::vec3& cameraPosition,
            const glm::vec3& cameraTarget,
            const glm::vec3& upVector
        ) {
            return glm::lookAt(
                cameraPosition,
                cameraTarget,
                upVector
            );
        }


        static inline glm::mat4 calculateProjectionMatrix(
            float fov,
            float aspectRatio,
            float nearPlane,
            float farPlane
        ) {
            return glm::perspective(
                glm::radians(fov),
                aspectRatio,
                nearPlane,
                farPlane
            );
        }


    public:

        Camera(
            GLFWwindow* window,
            const glm::vec3& cameraPosition,
            const glm::vec3& cameraTarget,
            float fov,
            float nearPlane,
            float farPlane
        ) {

            this->window = window;

            this->cameraPosition = cameraPosition;
            this->cameraTarget = cameraTarget;
            // We always use this vector to indicate the up direction
            upVector = glm::vec3(0.0f, 1.0f, 0.0f);
            this->fov = fov;

            // We can get the aspect ratio from the window size
            int width, height;
            glfwGetWindowSize(window, &width, &height);
            aspectRatio = width / static_cast<float>(height);
            this->nearPlane = nearPlane;
            this->farPlane = farPlane;

            viewMatrix = calculateViewMatrix(
                cameraPosition, 
                cameraTarget, 
                upVector
            );
            projectionMatrix = calculateProjectionMatrix(
                fov,
                aspectRatio,
                nearPlane,
                farPlane
            );
        }


        // Used in order to update the camera each frame
        virtual void processInput(float deltaT) = 0;


        glm::mat4 getViewMatrix() const {
            return viewMatrix;
        }


        glm::mat4 getProjectionMatrix() const {
            return projectionMatrix;
        }


        glm::vec3 getPosition() const {
            return cameraPosition;
        }


        void setPosition(glm::vec3& cameraPosition) {
            this->cameraPosition = cameraPosition;
        }


        glm::vec3 getForwarDirection() const {
            return glm::normalize(cameraTarget - cameraPosition);
        }


        float getFov() const {
            return fov;
        }


        float getFarPlane() const {
            return farPlane;
        }


        float getNearPlane() const {
            return nearPlane;
        }


        glm::vec3 getCameraTraget() const {
            return cameraTarget;
        }

	};
}

#endif