#ifndef POINT_PROJECTION_H
#define POINT_PROJECTION_H

#include "accuracy.h"

namespace pe {

    class PointProjection {

    private:

        // One projection matrix
        glm::mat4 projection;
        // One view matrix for each cubemap face
        std::vector<glm::mat4> view;

        glm::vec3 lightPos;
        float fovRadians;

        float nearPlane;
        float farPlane;


        // Update projection matrices for each cubemap face
        void updateProjection() {
            float aspectRatio = 1.0;
            projection = glm::perspective(
                fovRadians, aspectRatio, nearPlane, farPlane
            );
        }

        void updateView() {
            view = std::vector<glm::mat4>{
                glm::lookAt(lightPos, lightPos + glm::vec3(1, 0, 0), glm::vec3(0, -1, 0)), // +X
                glm::lookAt(lightPos, lightPos + glm::vec3(-1, 0, 0), glm::vec3(0, -1, 0)), // -X
                glm::lookAt(lightPos, lightPos + glm::vec3(0, 1, 0), glm::vec3(0, 0, 1)),   // +Y
                glm::lookAt(lightPos, lightPos + glm::vec3(0, -1, 0), glm::vec3(0, 0, -1)), // -Y
                glm::lookAt(lightPos, lightPos + glm::vec3(0, 0, 1), glm::vec3(0, -1, 0)),  // +Z
                glm::lookAt(lightPos, lightPos + glm::vec3(0, 0, -1), glm::vec3(0, -1, 0))  // -Z
            };
        }

    public:

        PointProjection(
            const glm::vec3& lightPos,
            float fovDegrees,
            float nearPlane,
            float farPlane
        ) :
            lightPos(lightPos), fovRadians(glm::radians(fovDegrees)),
            nearPlane(nearPlane), farPlane(farPlane)
        {
            updateProjection();
            updateView();
        }

        void setLightPosition(const glm::vec3& pos) {
            lightPos = pos;
            updateView();
        }

        const glm::mat4& getProjectionMatrix() const {
            return projection;
        }

        const std::vector<glm::mat4>& getViewMatrices() const {
            return view;
        }

        const glm::vec3& getLightPosition() const {
            return lightPos;
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
