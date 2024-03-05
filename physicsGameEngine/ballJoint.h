

#ifndef BALL_JOINT_H
#define BALL_JOINT_H

#include "RigidBody.h"

namespace pe {

    class BallJoint {
    private:
        RigidBody* body1;
        RigidBody* body2;
        Vector3D relativePoint1; // Relative point in body1's local coordinates
        Vector3D relativePoint2; // Relative point in body2's local coordinates
        real error; // Current positional error

    public:
        // Constructor
        BallJoint(
            RigidBody* b1, 
            RigidBody* b2, 
            const Vector3D& relPoint1, 
            const Vector3D& relPoint2,
            int error
        ) : body1(b1), 
            body2(b2), 
            relativePoint1(relPoint1), 
            relativePoint2(relPoint2), 
            error(error) {

            // Calculate the world positions of the local points
            Vector3D worldPoint1 = body1->getPointInWorldCoordinates(relPoint1);
            Vector3D worldPoint2 = body2->getPointInWorldCoordinates(relPoint2);

            // We initially move the bodies so that they are connected
            Vector3D direction = worldPoint2 - worldPoint1;
            body1->position += direction * 0.5;
            body2->position += direction * -0.5;
        
        }

        // Update the constraint (typically called once per frame)
        void update(real deltaTime) {

            static const real EPSILON = 0;

            // Convert relative points to world coordinates
            Vector3D worldPoint1 = body1->getPointInWorldCoordinates(relativePoint1);
            Vector3D worldPoint2 = body2->getPointInWorldCoordinates(relativePoint2);

            // Calculate the error (difference in positions)
            Vector3D delta = worldPoint2 - worldPoint1;
            error = delta.magnitude(); // Store the magnitude of the error

            // If error is significant, apply corrective forces or impulses to maintain the constraint
            if (error > EPSILON) {
                // Calculate correction force or impulse
                Vector3D correction = delta * (1 / error) * (error * deltaTime); // Scale by deltaTime for stability
                // Apply correction to both bodies
                body1->addForceAtLocalPoint(correction * 5000, relativePoint1);
                body2->addForceAtLocalPoint(correction * -5000, relativePoint2);
            }
        }

        // Get the positional error
        real getError() const {
            return error;
        }
    };
}


#endif