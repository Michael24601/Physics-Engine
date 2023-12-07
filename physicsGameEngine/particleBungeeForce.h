/*
	Header file for a particle elastic bungee force, which works similarly
	to a spring when elongated, but won't exert any force when compressed.
	Like the spring force, we can make an anchored version of it.
*/

#ifndef PARTICLE_BUNGEE_FORCE_H
#define PARTICLE_BUNGEE_FORCE_H

#include "particleForceGenerator.h"

namespace pe {

	class ParticleBungeeForce : public ParticleForceGenerator {

	public:

		// Other particle connected to the bungee rope (force not acting on it)
		Particle* otherParticle;

		// Spring constant k
		real springConstant;

		// Resting length of the bungee rope l0
		real restingLength;

		ParticleBungeeForce(Particle* otherParticle, real springConstant,
			real restingLength) : otherParticle{ otherParticle },
			springConstant{ springConstant }, restingLength{ restingLength } {};

		/*
			Follows the formula of force = -k * (| d | -l0) * d where d is
			the vector (position - otherObjecsPosition). This formula is only
			applicable when the rope is elongated, not compressed.
		*/
		virtual void updateForce(Particle* particle, real duration);
	};
}

#endif