
#ifndef CAMERA_H
#define CAMERA_H

#include <GL/gl.h>
#include <SFML/Graphics.hpp>
#include "accuracy.h"

namespace pe {

	class Camera {

    protected:

        // Position of the camera
        glm::vec3 cameraPosition;
        // Point the camera is looking at
        glm::vec3 cameraTarget;
        // The up vector indicating the basis we are in
        glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);

        // The view matrix of the camera, indicating where it is looking at
        glm::mat4 viewMatrix;

        // Field of View (FOV) in degrees
        real fov;
        // Aspect ratio
        real aspectRatio;
        // Near and far clipping planes
        real nearPlane;
        real farPlane;

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
            real fov,
            real aspectRatio,
            real nearPlane,
            real farPlane
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
            const sf::Window& window,
            const glm::vec3& cameraPosition,
            const glm::vec3& cameraTarget,
            real fov,
            real nearPlane,
            real farPlane
        ) {
            this->cameraPosition = cameraPosition;
            this->cameraTarget = cameraTarget;
            // We always use this vector to indicate the up direction
            upVector = glm::vec3(0.0f, 1.0f, 0.0f);
            this->fov = fov;
            // We can get the aspect ratio from the window size
            aspectRatio = window.getSize().x / static_cast<real>(window.getSize().y);
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


        // Used in order to update the camera using events each frame
        virtual void update(sf::Event& event, real deltaT) = 0;


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
	};
}

#endif