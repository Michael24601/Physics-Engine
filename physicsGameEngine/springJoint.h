
#ifndef SPRING_JOINT_H
#define SPRING_JOINT_H

#include "rigidBody.h"
#include "rigidBodySpringForce.h"
#include "rigidBodyBungeeForce.h"

namespace pe {

    static int stiffness = 50;

    class SpringJoint {

    private:

        RigidBody* body1;  // Pointer to the first rigid body
        RigidBody* body2;  // Pointer to the second rigid body
        Vector3D localPoint1;  // Local point on body1
        Vector3D localPoint2;  // Local point on body2
        real distance;  // Desired distance between local points

        RigidBodySpringForce spring1;
        RigidBodySpringForce spring2;

    public:

        SpringJoint(
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
            distance(dist),
            spring1(lp1, b2, lp2, stiffness, dist),
            spring2(lp2, b1, lp1, stiffness, dist) 
        {

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
            spring1.updateForce(body1, deltaT);
            spring2.updateForce(body2, deltaT);
        }
    };
}

#endif