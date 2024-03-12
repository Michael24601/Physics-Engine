
#ifndef BALL_JOINT_H
#define BALL_JOINT_H

#include "rigidBody.h"

namespace pe {

    class BallJoint {
    private:
        RigidBody* body1;
        RigidBody* body2;
        Vector3D anchor1;
        Vector3D anchor2;

    public:
        BallJoint(RigidBody* rb1, RigidBody* rb2, const Vector3D& localAnchor1, const Vector3D& localAnchor2)
            : body1(rb1), anchor1(localAnchor1), body2(rb2), anchor2(localAnchor2) {}

        // Apply constraint at each time step
        void update(real duration) {
            // Convert anchor points from local to world space
            Vector3D worldAnchor1 = body1->getPointInWorldCoordinates(anchor1);
            Vector3D worldAnchor2 = body2->getPointInWorldCoordinates(anchor2);

            // Calculate relative position of anchor points
            Vector3D relativePosition = worldAnchor2 - worldAnchor1;

            // Calculate relative velocity of anchor points
            Vector3D relativeVelocity = body2->linearVelocity + body2->angularVelocity.vectorProduct(anchor2)
                - body1->linearVelocity - body1->angularVelocity.vectorProduct(anchor1);

            // Calculate damping impulse to stabilize motion
            Vector3D dampingImpulse = relativeVelocity * 0.1;

            // Calculate desired relative position (zero relative displacement)
            Vector3D correctiveImpulse = (relativePosition * (1 / duration)) + dampingImpulse;


            // Apply corrective impulse to each body
            body1->addForceAtLocalPoint(correctiveImpulse, anchor1);
            body2->addForceAtLocalPoint(correctiveImpulse*-1, anchor2);
        }
    };

}


#endif