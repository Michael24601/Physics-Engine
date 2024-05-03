
#ifndef FIRST_PERSON_CAMERA_H
#define FIRST_PERSON_CAMERA_H

#include "camera.h"

namespace pe {

    class FirstPersonCamera : public Camera {

    private:

        // Indicates how fast the camera moves
        float movementSpeed;
        float rotationSpeed;

    public:

        FirstPersonCamera(
            GLFWwindow* window,
            const glm::vec3& cameraPosition,
            const glm::vec3& cameraTarget,
            float fov,
            float nearPlane,
            float farPlane,
            float movementSpeed,
            float rotationSpeed
        ) : Camera(
            window,
            cameraPosition,
            cameraTarget,
            fov,
            nearPlane,
            farPlane
        ), movementSpeed{ movementSpeed },
            rotationSpeed{rotationSpeed} {}


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


            float rSpeed{ 0 };
            // The further it is is the more the rotation
            if (xpos < (width / 5.0)) {
                float multiplier = (width / 5.0 - xpos) / 200.0;
                rSpeed = rotationSpeed * duration * multiplier;
            }
            else if (xpos > (width - width / 5.0)) {
                float multiplier = (xpos - (width - width / 5.0)) / 200.0;
                rSpeed = -rotationSpeed * duration * multiplier;
            }

            // Rotate the forward vector around the up vector
            glm::mat4 horizontalRotationMatrix = glm::rotate(glm::mat4(1.0f), rSpeed, upVector);
            glm::vec3 newForward = glm::normalize(glm::vec3(horizontalRotationMatrix * glm::vec4(forward, 0.0f)));

            // Calculate new right vector based on new forward vector
            glm::vec3 newRight = glm::normalize(glm::cross(newForward, upVector));

            // Calculate new camera target position
            glm::vec3 offset = cameraTarget - cameraPosition;
            glm::vec3 newOffset = glm::normalize(newForward) * glm::length(offset);

            // Update camera target based on horizontal rotation
            cameraTarget = cameraPosition + newOffset;


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
