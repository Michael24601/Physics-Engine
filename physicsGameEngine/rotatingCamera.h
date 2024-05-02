
#ifndef ROTATING_CAMERA_H
#define ROTATING_CAMERA_H

#include "camera.h"

namespace pe {

	class RotatingCamera : public Camera {

	private:

		// Indicates how fast the camera moves
		float distanceFromCentre;
		float movementSpeed;
		float rotationSpeed;
		float angle;

	public:

		RotatingCamera(
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
		), rotationSpeed{ rotationSpeed },
			movementSpeed{movementSpeed},
			// Initial value
			angle{ 0 }{

			glm::vec3 horizontalPosition(cameraPosition.x, 0, cameraPosition.z);
			distanceFromCentre = glm::length(horizontalPosition);
		}


		void processInput(float duration) override {

			// Updating camera rotation based on arrow keys pressed
			if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
				angle += rotationSpeed;
				cameraPosition.x = sin(angle) * distanceFromCentre;
				cameraPosition.z = cos(angle) * distanceFromCentre;
				viewMatrix = glm::lookAt(cameraPosition, cameraTarget, upVector);
			}
			else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
				angle -= rotationSpeed;
				cameraPosition.x = sin(angle) * distanceFromCentre;
				cameraPosition.z = cos(angle) * distanceFromCentre;
				viewMatrix = glm::lookAt(cameraPosition, cameraTarget, upVector);
			}

			// Moves camera
			// Rotates camera (only x and z are moved, y stays in place)
			if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
				distanceFromCentre *= (1 - movementSpeed);
				cameraPosition.x *= (1 - movementSpeed);
				cameraPosition.z *= (1 - movementSpeed);
				viewMatrix = glm::lookAt(cameraPosition, cameraTarget, upVector);
			}
			else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
				distanceFromCentre *= (1 + movementSpeed);
				cameraPosition.x *= (1 + movementSpeed);
				cameraPosition.z *= (1 + movementSpeed);
				viewMatrix = glm::lookAt(cameraPosition, cameraTarget, upVector);
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