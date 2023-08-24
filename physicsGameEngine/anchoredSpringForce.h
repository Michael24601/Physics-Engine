/*
	Another spring force class that connects an object to an anchor
	(immovable object) instead of connecting two objects.
*/

#ifndef ANCHORED_SPRING_FORCE_H
#define ANCHORED_SPRING_FORCE_H

#include "particleForceGenerator.h"

namespace pe {

	class AnchoredSpringForce : public ParticleForceGenerator {

	private:

		// Vector for the position of the anchor
		Vector3D* anchor;

		// Spring constant k
		real springConstant;

		// Resting length of the spring l0
		real restingLength;

	public:

		AnchoredSpringForce(Vector3D* anchor, real springConstant,
			real restingLength) : anchor{ anchor },
			springConstant{ springConstant }, 
			restingLength{ restingLength } {};

		/*
			Follows the formula of force = -k * (| d | -l0) * d where d is
			the vector (position - anchorPosition)
		*/
		virtual void updateForce(Particle* particle, real duration);
	};
}

#endif