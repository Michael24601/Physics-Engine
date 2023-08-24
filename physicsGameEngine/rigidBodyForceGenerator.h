/*
	Rigid Body force interface that generalizes the way a force affects a
	rigid Body.
*/

#ifndef RIGID_BODY_FORCE_GENERATOR_H
#define RIGID_BODY_FORCE_GENERATOR_H

#include "vector3D.h"
#include "rigidBody.h"

namespace pe {

	class RigidBodyForceGenerator {
	public:

		/*
			Adds a force to a rigid body by changing the object somehow.
			This force is automatically removed from the accumulator, but
			can then be added again on the next frame. Because the
			updateForce function can have an effect on the acceleration,
			based on multiple parameters like velocity, mass, etc... we also
			include the duration parameter in case the frame size affects
			how the force affects the body.
		*/
		virtual void updateForce(RigidBody* body, real duration) = 0;
	};
}

#endif