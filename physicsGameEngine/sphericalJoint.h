
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

        void update(real deltaT) {

            static const real stiffness = 10;

            // Calculate world space anchor points
            Vector3D worldAnchorA = body1->getPointInWorldCoordinates(relativePointBody1);
            Vector3D worldAnchorB = body2->getPointInWorldCoordinates(relativePointBody2);

            // Calculate relative position between bodies
            Vector3D relativePosition = worldAnchorB - worldAnchorA;

            // Constrain relative position to maintain connection
            Vector3D constraintForce = relativePosition * stiffness;

            // Apply constraint force to bodies
            body1->addForce(constraintForce);
            body2->addForce(constraintForce*-1);
        }
    };
}

#endif
