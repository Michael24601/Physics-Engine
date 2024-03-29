
#ifndef ROTATING_CAMERA_H
#define ROTATING_CAMERA_H

#include "camera.h"

namespace pe {

	class RotatingCamera : public Camera {

	private:

		// Indicates how fast the camera moves
		real distanceFromCentre;
		real movementSpeed;
		real rotationSpeed;
		real angle;

	public:

		RotatingCamera(
			const sf::Window& window,
			const glm::vec3& cameraTarget,
			real fov,
			real nearPlane,
			real farPlane,
			real distanceFromCentre,
			real movementSpeed,
			real rotationSpeed
		) : Camera(
			window,
			// The position is pre-set
			glm::vec3(0, 0, distanceFromCentre),
			cameraTarget,
			fov,
			nearPlane,
			farPlane
		), rotationSpeed{ rotationSpeed },
			movementSpeed{movementSpeed},
			distanceFromCentre{distanceFromCentre},
			// Initial value
			angle{ 0 }{}


		void update(sf::Event& event, real duration) {

			float deltaT = static_cast<float>(duration);
			float speed = static_cast<float>(rotationSpeed);

			// The direction the camera is facing based on the view matrix
			glm::vec3 forward = glm::normalize(cameraTarget - cameraPosition);

			// Updating camera rotation based on arrow keys pressed
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
				angle += rotationSpeed;
				cameraPosition.x = sin(angle) * distanceFromCentre;
				cameraPosition.z = cos(angle) * distanceFromCentre;
				viewMatrix =
					glm::lookAt(cameraPosition, cameraTarget, upVector);
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
				angle -= rotationSpeed;
				cameraPosition.x = sin(angle) * distanceFromCentre;
				cameraPosition.z = cos(angle) * distanceFromCentre;
				viewMatrix =
					glm::lookAt(cameraPosition, cameraTarget, upVector);
			}
			// Moves camera
			 // Rotates camera
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
				distanceFromCentre *= (1-movementSpeed);
				cameraPosition *= (1 - movementSpeed);
				viewMatrix =
					glm::lookAt(cameraPosition, cameraTarget, upVector);
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
				distanceFromCentre *= (1 + movementSpeed);
				cameraPosition *= (1 + movementSpeed);
				viewMatrix =
					glm::lookAt(cameraPosition, cameraTarget, upVector);
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