
#ifndef BALL_JOINT_H
#define BALL_JOINT_H

#include "rigidBody.h"

namespace pe {

    class BallJoint {

    private:

        RigidBody* body1;  // Pointer to the first rigid body
        RigidBody* body2;  // Pointer to the second rigid body
        Vector3D localPoint1;  // Local point on body1
        Vector3D localPoint2;  // Local point on body2
        real distance;  // Desired distance between local points

    public:

        BallJoint(
            RigidBody* b1,
            RigidBody* b2,
            const Vector3D& lp1,
            const Vector3D& lp2,
            real dist
        )
            : body1(b1),
            body2(b2),
            localPoint1(lp1),
            localPoint2(lp2),
            distance(dist) {

            // Calculates the world positions of the local points
            Vector3D worldPoint1 = body1->getPointInWorldCoordinates(localPoint1);
            Vector3D worldPoint2 = body2->getPointInWorldCoordinates(localPoint2);

            /*
                We initially move the bodies so that they are connected at
                the desired distance. This avoids the initial jerk that may
                caused instability.
            */
            Vector3D direction = (worldPoint2 - worldPoint1);
            real currentDistance = direction.magnitude();
            direction.normalize();
            real displacement = currentDistance - distance;
            Vector3D correction = direction * displacement;

            // Moves the bodies in opposite directions by half the correction distance
            body1->position += correction * 0.5;
            body2->position -= correction * 0.5;
        }

        // Update the joint constraints
        void update(real deltaT) {

            static const real stiffness = 1000;

            // Calculate the world positions of the local points
            Vector3D worldPoint1 = body1->getPointInWorldCoordinates(localPoint1);
            Vector3D worldPoint2 = body2->getPointInWorldCoordinates(localPoint2);

            // Calculates the relative position of the two points
            Vector3D relativePosition = worldPoint2 - worldPoint1;

            // Calculates the current distance between the points
            real currentDistance = relativePosition.magnitude();

            // Calculates the error in distance
            real error = currentDistance - distance;

            // Calculates the force to correct the error (Hooke's Law)
            Vector3D force = relativePosition * (error * stiffness);

            // Applies equal and opposite forces to maintain the connection
            // body1->addForce(force);
            // body2->addForce(force * -1);

            /*
                One issue with this approach is that the force is always
                applied to the centre of the bodies. So if one body is
                dangling from another, and the second body only has gravity
                (also applies to the centre) acting on it, it won't rotate
                at all, and will keep its original orientation, instead of
                dangling.
            */

            body1->angularDamping = 0.05;
            body1->addForceAtLocalPoint(force * body1->inverseMass, localPoint1);
            body2->addForceAtLocalPoint(force * -1 * body1->inverseMass, localPoint2);

        }
    };
}

#endif