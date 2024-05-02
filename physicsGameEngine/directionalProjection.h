/*
    Header file for a class representing a directional projection.
    Can be used for directional light.
*/

#ifndef DIRECTIONAL_PROJECTION_H
#define DIRECTIONAL_PROJECTION_H

#include "accuracy.h"

namespace pe {

    class DirectionalProjection {

    private:

        glm::mat4 projection;
        glm::mat4 view;

        glm::vec3 lightPosition;
        float frustumSize;

        int sourceWidth;
        int sourceHeight;
        float nearPlane;
        float farPlane;


        void updateProjection() {
            float aspectRatio = (float)sourceWidth / (float)sourceHeight;
            projection = glm::ortho(
                -frustumSize * aspectRatio,
                frustumSize * aspectRatio,
                -frustumSize, frustumSize,
                nearPlane,
                farPlane
            );
        }


        void updateView() {
            view = glm::lookAt(
                lightPosition,
                glm::vec3(0.0f),
                glm::vec3(0.0f, 1.0f, 0.0f)
            );
        }

    public:

        DirectionalProjection(
            const glm::vec3& lightPos,
            float size, 
            int sourceWidth, 
            int sourceHeight,
            float nearPlane,
            float farPlane
        ) :
            lightPosition(lightPos), frustumSize(size),
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


        glm::mat4 getProjection() const {
            return projection;
        }


        glm::mat4 getView() const {
            return view;
        }


        glm::mat4 getProjectionView() const {
            return projection * view;
        }
   
    };
}

#endif