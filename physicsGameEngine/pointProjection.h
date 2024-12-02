#ifndef POINT_PROJECTION_H
#define POINT_PROJECTION_H

#include "accuracy.h"

namespace pe {

    class PointProjection {

    private:

        // One projection matrix for each cubemap face
        glm::mat4 projection[6];
        // One view matrix for each cubemap face
        glm::mat4 view[6];

        glm::vec3 lightPosition;
        float fovRadians;

        int sourceWidth;
        int sourceHeight;
        float nearPlane;
        float farPlane;


        // Update projection matrices for each cubemap face
        void updateProjection() {
            for (int i = 0; i < 6; ++i) {
                float aspectRatio = 1.0;
                projection[i] = glm::perspective(
                    fovRadians, aspectRatio, nearPlane, farPlane
                );
            }
        }

        void updateView() {
            view[0] = glm::lookAt(lightPosition, lightPosition + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));  // Positive X
            view[1] = glm::lookAt(lightPosition, lightPosition + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Negative X
            view[2] = glm::lookAt(lightPosition, lightPosition + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));  // Positive Y
            view[3] = glm::lookAt(lightPosition, lightPosition + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)); // Negative Y
            view[4] = glm::lookAt(lightPosition, lightPosition + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));  // Positive Z
            view[5] = glm::lookAt(lightPosition, lightPosition + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Negative Z
        }

    public:

        PointProjection(
            const glm::vec3& lightPos,
            float fovDegrees,
            int sourceWidth,
            int sourceHeight,
            float nearPlane,
            float farPlane
        ) :
            lightPosition(lightPos), fovRadians(glm::radians(fovDegrees)),
            sourceWidth(sourceWidth), sourceHeight(sourceHeight),
            nearPlane(nearPlane), farPlane(farPlane)
        {
            updateProjection();
            updateView();
        }

        void setLightPosition(const glm::vec3& pos) {
            lightPosition = pos;
            updateView();
        }

        glm::mat4 getProjection(int faceIndex) const {
            return projection[faceIndex];
        }

        glm::mat4 getView(int faceIndex) const {
            return view[faceIndex];
        }

        glm::mat4 getProjectionView(int faceIndex) const {
            return projection[faceIndex] * view[faceIndex];
        }

        int getWidth() const {
            return sourceWidth;
        }

        int getHeight() const {
            return sourceHeight;
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
