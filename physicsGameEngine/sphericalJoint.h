
#ifndef SPHERICAL_JOINT_H
#define SPHERICAL_JOINT_H

#include "rigidBody.h"

namespace pe {

    class SphericalJoint {
    private:
        RigidBody* body1;
        RigidBody* body2;
        Vector3D relativePointBody1;
        Vector3D relativePointBody2;

    public:
        SphericalJoint(RigidBody* body1, RigidBody* body2,
            const Vector3D& relativePointBody1,
            const Vector3D& relativePointBody2)
            : body1(body1), body2(body2),
            relativePointBody1(relativePointBody1),
            relativePointBody2(relativePointBody2) {
        

            // Calculates the world positions of the local points
            Vector3D worldPoint1 = body1->getPointInWorldCoordinates(relativePointBody1);
            Vector3D worldPoint2 = body2->getPointInWorldCoordinates(relativePointBody2);

            /*
                We initially move the bodies so that they are connected at
                the desired distance. This avoids the initial jerk that may
                caused instability.
            */
            Vector3D direction = (worldPoint2 - worldPoint1);
            real currentDistance = direction.magnitude();
            direction.normalize();
            real displacement = currentDistance - 0;
            Vector3D correction = direction * displacement;

            // Moves the bodies in opposite directions by half the correction distance
            body1->position += correction * 0.5;
            body2->position -= correction * 0.5;
        }

        void update(real deltaTime) {
            // Transform relative points to world space
            Vector3D worldPointBody1 = body1->getPointInWorldCoordinates(relativePointBody1);
            Vector3D worldPointBody2 = body2->getPointInWorldCoordinates(relativePointBody2);

            // Calculate the vector between the two points
            Vector3D relativePosition = worldPointBody2 - worldPointBody1;

            // Apply a force to each body to keep the relative position constant
            body1->addForceAtLocalPoint(relativePosition * body1->inverseMass * 10000, relativePointBody1);
            body2->addForceAtLocalPoint(relativePosition * body2->inverseMass * -10000, relativePointBody2);
        }
    };
}

#endif
