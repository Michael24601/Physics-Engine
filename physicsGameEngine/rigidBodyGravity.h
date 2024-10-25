/*
	Header file for a gravity force, but this time applied to a rigid body.
	The force can be added directly into the body class in the acceleration
	variable, but there is always the option of adding it manually.
*/

#ifndef RIGID_BODY_GRAVITY_H
#define RIGID_BODY_GRAVITY_H

#include "accuracy.h"
#include "vector3D.h"
#include "rigidBodyForceGenerator.h"

namespace pe {

	class RigidBodyGravity : public RigidBodyForceGenerator {

	private:

		// Vector of gravity, which should be (0, -g, 0)
		Vector3D gravity;

	public:

		RigidBodyGravity(const Vector3D& gravity) : gravity{ gravity } {};

		virtual void updateForce(RigidBody* body, real duration) const override;
	};
}

#endif