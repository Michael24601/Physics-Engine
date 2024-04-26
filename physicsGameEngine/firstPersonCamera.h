
#ifndef FIRST_PERSON_CAMERA_H
#define FIRST_PERSON_CAMERA_H

#include "camera.h"

namespace pe {

    class FirstPersonCamera : public Camera {

    private:

        // Indicates how fast the camera moves
        float movementSpeed;
        glm::vec2 lastCursorPosition;

    public:

        FirstPersonCamera(
            GLFWwindow* window,
            const glm::vec3& cameraPosition,
            const glm::vec3& cameraTarget,
            float fov,
            float nearPlane,
            float farPlane,
            float movementSpeed
        ) : Camera(
            window,
            cameraPosition,
            cameraTarget,
            fov,
            nearPlane,
            farPlane
        ), movementSpeed{ movementSpeed }, lastCursorPosition{ 0.0f } {}


        void processInput(float duration) override {

            // The direction the camera is facing based on the view matrix
            glm::vec3 forward = glm::normalize(cameraTarget - cameraPosition);
            // The right direction, used to move sideways
            glm::vec3 right = glm::normalize(glm::cross(forward, upVector));

            // Get current cursor position
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            int width, height;
            glfwGetFramebufferSize(window, &width, &height);
            glm::vec2 cursorPosition(
                xpos - width/ 2.0f,
                -ypos + height / 2.0f
            );

            // Calculate the change in cursor position
            glm::vec2 cursorDelta = cursorPosition - lastCursorPosition;

            // Update last cursor position
            lastCursorPosition = cursorPosition;

            // Updating camera position based on arrow keys pressed
            if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
                cameraPosition += forward * movementSpeed * duration;
                cameraTarget += forward * movementSpeed * duration;
            }
            if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
                cameraPosition -= forward * movementSpeed * duration;
                cameraTarget -= forward * movementSpeed * duration;
            }
            if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
                cameraPosition -= right * movementSpeed * duration;
                cameraTarget -= right * movementSpeed * duration;
            }
            if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
                cameraPosition += right * movementSpeed * duration;
                cameraTarget += right * movementSpeed * duration;
            }

            // Calculate horizontal rotation based on mouse movement
            float horizontalRotation = -cursorDelta.x / (float)width;

            // Rotate the forward vector around the up vector
            glm::mat4 horizontalRotationMatrix = glm::rotate(glm::mat4(1.0f), horizontalRotation, upVector);
            glm::vec3 newForward = glm::normalize(glm::vec3(horizontalRotationMatrix * glm::vec4(forward, 1.0f)));

            // Update camera target based on horizontal rotation
            cameraTarget = cameraPosition + newForward;

            // Update view matrix
            viewMatrix = calculateViewMatrix(
                cameraPosition,
                cameraTarget,
                upVector
            );
        }

    };
}

#endif
