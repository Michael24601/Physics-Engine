
#ifndef PARTICLE_SPRING_DAMPER_H
#define PARTICLE_SPRING_DAMPER_H

#include "particleForceGenerator.h"

namespace pe {

	class ParticleSpringDamper : public ParticleForceGenerator {

	public:

		// Other particle connected to the spring (force not acting on it)
		Particle* otherParticle;

		// Spring constant ks
		real springConstant;

		// Damping constant kd
		real dampingConstant;

		// Resting length of the spring l0
		real restingLength;

	public:

		ParticleSpringDamper(
			Particle* otherParticle, 
			real springConstant,
			real dampingConstant,
			real restingLength
		) : otherParticle{ otherParticle },
			springConstant{ springConstant },
			dampingConstant{dampingConstant},
			restingLength{ restingLength } {};

		/*
			Follows the formula of force = -k * (| d | -l0) * d where d is
			the vector (position - otherObjecsPosition)
		*/
		void updateForce(Particle* particle, real duration) const override;
	};
}


#endif