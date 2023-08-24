/*
	Header file for a gravity force. The gravity force can be harcoded
	into any particle, using a base acceleration of (0, -g, 0), on top
	of which any forces are added. However, we can also create a gravity
	force which is added at each iteration, and leave the acceleration
	variable at (0, 0, 0) in each particle.
*/

#ifndef PARTICLE_GRAVITY_H
#define PARTICLE_GRAVITY_H

#include "accuracy.h"
#include "vector3D.h"
#include "particleForceGenerator.h"

namespace pe {

	class ParticleGravity : public ParticleForceGenerator {

	private:

		// Vector of gravity, which should be (0, -g, 0)
		Vector3D gravity;

	public:

		ParticleGravity(const Vector3D& gravity) : gravity{ gravity } {};

		virtual void updateForce(Particle* particle, real duration);
	};
}

#endif