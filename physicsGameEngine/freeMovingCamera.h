
#ifndef FREE_MOVING_CAMERA_H
#define FREE_MOVING_CAMERA_H

#include "camera.h"

namespace pe {

	class FreeMovingCamera : public Camera {

	private:

		// Indicates how fast the camera moves
		real movementSpeed;
		real rotationSpeed;

	public:

		FreeMovingCamera(
			const sf::Window& window,
			const glm::vec3& cameraPosition,
			const glm::vec3& cameraTarget,
			real fov,
			real nearPlane,
			real farPlane,
			real movementSpeed,
			real rotationSpeed
		) : Camera(
			window,
			cameraPosition,
			cameraTarget,
			fov,
			nearPlane,
			farPlane
		), movementSpeed{ movementSpeed },
			rotationSpeed{rotationSpeed} {}


		void update(sf::Event& event, real duration) {

			float deltaT = static_cast<float>(duration);
			float speed = static_cast<float>(movementSpeed);
			float rSpeed = static_cast<float>(rotationSpeed);

			// The direction the camera is facing based on the view matrix
			glm::vec3 forward = glm::normalize(cameraTarget - cameraPosition);
			// The right direction, used to move sideways
			glm::vec3 right = glm::normalize(glm::cross(forward, upVector));

			// Updating camera position based on keys pressed
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
				cameraPosition += forward * speed * deltaT;
				cameraTarget += forward * speed * deltaT;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
				cameraPosition -= forward * speed * deltaT;
				cameraTarget -= forward * speed * deltaT;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
				cameraPosition -= right * speed * deltaT;
				cameraTarget -= right * speed * deltaT;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
				cameraPosition += right * speed * deltaT;
				cameraTarget += right * speed * deltaT;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) {
				cameraPosition += upVector * speed * deltaT;
				cameraTarget += upVector * speed * deltaT;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::X)) {
				cameraPosition -= upVector * speed * deltaT;
				cameraTarget -= upVector * speed * deltaT;
			}

			// Updating camera rotation based on arrow keys pressed
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
				glm::quat rotation = glm::angleAxis(rSpeed * deltaT, upVector);
				glm::vec3 newForward = glm::normalize(rotation * forward);
				cameraTarget = cameraPosition + newForward;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
				glm::quat rotation = glm::angleAxis(-rSpeed * deltaT, upVector);
				glm::vec3 newForward = glm::normalize(rotation * forward);
				cameraTarget = cameraPosition + newForward;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
				glm::vec3 rotationAxis = glm::normalize(glm::cross(forward, upVector));
				glm::mat4 rotationMatrix = glm::rotate(
					glm::mat4(1.0f), rSpeed * deltaT, rotationAxis
				);

				glm::vec4 newForward4 = rotationMatrix * glm::vec4(forward, 1.0f);
				glm::vec3 newForward = glm::vec3(newForward4);
				cameraTarget = cameraPosition + newForward;

				glm::vec4 newUp4 = rotationMatrix * glm::vec4(upVector, 1.0f);
				upVector = glm::vec3(newUp4);
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
				// Rotate camera down
				glm::vec3 rotationAxis = glm::normalize(glm::cross(forward, upVector));
				glm::mat4 rotationMatrix = glm::rotate(
					glm::mat4(1.0f), -rSpeed * deltaT, rotationAxis
				);

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