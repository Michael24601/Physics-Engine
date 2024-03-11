
#ifndef FIXED_JOINT_H
#define FIXED_JOINT_H

#include "rigidBody.h"

namespace pe {

	class FixedJoint {

	private:

		Vector3D localAnchorA;
		Vector3D localAnchorB;
		RigidBody* bodyA;
		RigidBody* bodyB;

	public:

		FixedJoint(
			RigidBody* rbA, 
			RigidBody* rbB, 
			const Vector3D& anchorA,
			const Vector3D& anchorB
		) : bodyA(rbA), 
			bodyB(rbB), 
			localAnchorA(anchorA), 
			localAnchorB(anchorB) {
		
			// Calculates the world positions of the local points
			Vector3D worldPoint1 = rbA->getPointInWorldCoordinates(anchorA);
			Vector3D worldPoint2 = rbB->getPointInWorldCoordinates(anchorB);

			Vector3D direction = (worldPoint2 - worldPoint1);

			// Moves the bodies in opposite directions by half the correction distance
			rbA->position += direction * 0.5;
			rbB->position -= direction * 0.5;

		}

		// Update the joint (calculate constraint forces)
		void update(real duration) {
			// Calculate world space anchors
			Vector3D worldAnchorA = bodyA->getPointInWorldCoordinates(localAnchorA);
			Vector3D worldAnchorB = bodyB->getPointInWorldCoordinates(localAnchorB);

			// Calculate constraint force
			Vector3D deltaPosition = worldAnchorB - worldAnchorA;
			real constraintMass = bodyA->inverseMass + bodyB->inverseMass;
			if (constraintMass == 0) return; // Avoid division by zero
			real lambda = -(deltaPosition.scalarProduct(deltaPosition) / constraintMass);

			// Apply impulse
			Vector3D impulse = deltaPosition * lambda;
			bodyA->position -= impulse * bodyA->inverseMass;
			bodyB->position += impulse * bodyB->inverseMass;
		}

		// Set local anchors
		void setLocalAnchors(const Vector3D& anchorA, const Vector3D& anchorB) {
			localAnchorA = anchorA;
			localAnchorB = anchorB;
		}

		// Set connected rigid bodies
		void setConnectedBodies(RigidBody* rbA, RigidBody* rbB) {
			bodyA = rbA;
			bodyB = rbB;
		}
    };
}

#endif