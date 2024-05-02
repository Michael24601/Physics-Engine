
#ifndef POINT_PROJECTION_H
#define POINT_PROJECTION_H

#include "accuracy.h"

namespace pe {

    class PointProjection {

    private:

        glm::mat4 projection;
        // For each face of the cube map
        glm::mat4 views[6];
        glm::vec3 lightPosition;
        float nearPlane;
        float farPlane;

        void updateProjection() {
            projection = glm::perspective(glm::radians(90.0f), 1.0f, nearPlane, farPlane);
        }

        void updateViews() {
            // X
            views[0] = glm::lookAt(
                lightPosition, 
                lightPosition + glm::vec3(1.0f, 0.0f, 0.0f), 
                glm::vec3(0.0f, -1.0f, 0.0f)
            );
            // -X
            views[1] = glm::lookAt(
                lightPosition, 
                lightPosition + glm::vec3(-1.0f, 0.0f, 0.0f),
                glm::vec3(0.0f, -1.0f, 0.0f)
            );
            // Y
            views[2] = glm::lookAt(
                lightPosition, 
                lightPosition + glm::vec3(0.0f, 1.0f, 0.0f), 
                glm::vec3(0.0f, 0.0f, 1.0f)
            );
            // -Y
            views[3] = glm::lookAt(
                lightPosition, 
                lightPosition + glm::vec3(0.0f, -1.0f, 0.0f),
                glm::vec3(0.0f, 0.0f, -1.0f)
            );
            // Z
            views[4] = glm::lookAt(
                lightPosition, 
                lightPosition + glm::vec3(0.0f, 0.0f, 1.0f), 
                glm::vec3(0.0f, -1.0f, 0.0f)
            );
            // -Z
            views[5] = glm::lookAt(
                lightPosition, 
                lightPosition + glm::vec3(0.0f, 0.0f, -1.0f), 
                glm::vec3(0.0f, -1.0f, 0.0f)
            );
        }

    public:

        PointProjection(
            const glm::vec3& lightPos,
            float nearPlane,
            float farPlane
        ) :
            lightPosition(lightPos), nearPlane(nearPlane), farPlane(farPlane)
        {
            updateProjection();
            updateViews();
        }

        void setLightPosition(const glm::vec3& pos) {
            lightPosition = pos;
            updateViews();
        }

        glm::mat4 getProjection() const {
            return projection;
        }

        glm::mat4 getView(int faceIndex) const {
            if (faceIndex >= 0 && faceIndex < 6) {
                return views[faceIndex];
            }
            else {
                // Identity is returned if faceIndex is out of range
                return glm::mat4(1.0f);
            }
        }

        glm::mat4 getProjectionView(int faceIndex) const {
            if (faceIndex >= 0 && faceIndex < 6) {
                return projection * views[faceIndex];
            }
            else {
                // Identity is returned if faceIndex is out of range
                return glm::mat4(1.0f);
            }
        }
    };
}

#endif
