
#ifndef FREE_MOVING_CAMERA_H
#define FREE_MOVING_CAMERA_H

#include "camera.h"

namespace pe {

	class FreeMovingCamera : public Camera {

	private:

		// Indicates how fast the camera moves
		float movementSpeed;
		float rotationSpeed;

	public:

		FreeMovingCamera(
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

			// Updating camera position based on keys pressed
			if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
				cameraPosition += forward * movementSpeed * duration;
				cameraTarget += forward * movementSpeed * duration;
			}
			if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
				cameraPosition -= forward * movementSpeed * duration;
				cameraTarget -= forward * movementSpeed * duration;
			}
			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
				cameraPosition -= right * movementSpeed * duration;
				cameraTarget -= right * movementSpeed * duration;
			}
			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
				cameraPosition += right * movementSpeed * duration;
				cameraTarget += right * movementSpeed * duration;
			}
			if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
				cameraPosition += upVector * movementSpeed * duration;
				cameraTarget += upVector * movementSpeed * duration;
			}
			if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
				cameraPosition -= upVector * movementSpeed * duration;
				cameraTarget -= upVector * movementSpeed * duration;
			}

			// Updating camera rotation based on arrow keys pressed
			if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
				glm::quat rotation = glm::angleAxis(rotationSpeed * duration, upVector);
				glm::vec3 newForward = glm::normalize(rotation * forward);
				cameraTarget = cameraPosition + newForward;
			}
			if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
				glm::quat rotation = glm::angleAxis(-rotationSpeed * duration, upVector);
				glm::vec3 newForward = glm::normalize(rotation * forward);
				cameraTarget = cameraPosition + newForward;
			}
			if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
				glm::vec3 rotationAxis = glm::normalize(glm::cross(forward, upVector));
				glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), rotationSpeed * duration, rotationAxis);

				glm::vec4 newForward4 = rotationMatrix * glm::vec4(forward, 1.0f);
				glm::vec3 newForward = glm::vec3(newForward4);
				cameraTarget = cameraPosition + newForward;

				glm::vec4 newUp4 = rotationMatrix * glm::vec4(upVector, 1.0f);
				upVector = glm::vec3(newUp4);
			}
			if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
				glm::vec3 rotationAxis = glm::normalize(glm::cross(forward, upVector));
				glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), -rotationSpeed * duration, rotationAxis);

				glm::vec4 newForward4 = rotationMatrix * glm::vec4(forward, 1.0f);
				glm::vec3 newForward = glm::vec3(newForward4);
				cameraTarget = cameraPosition + newForward;

				glm::vec4 newUp4 = rotationMatrix * glm::vec4(upVector, 1.0f);
				upVector = glm::vec3(newUp4);
			}

			viewMatrix = calculateViewMatrix(
				cameraPosition,
				cameraTarget,
				upVector
			);
		}

	};
}

#endif