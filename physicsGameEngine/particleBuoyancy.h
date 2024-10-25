/* 
	Header file for the particle buoyancy force, the force exerted by
	a liquid on a floating oject based on its density and the volume of
	the object submerged. Since particles have no volume, based on the
	depth of the particle, a spring like force is used to approximate the
	buoyancy force.
*/

#ifndef PARTICLE_BUOYANCY_H
#define PARTICLE_BUOYANCY_H

#include "particleForceGenerator.h"

namespace pe {

	class ParticleBuoyancy : public ParticleForceGenerator {

	private:

		// Height of the liquid surface (liquid level above the y = 0 line)
		real liquidHeight;

		/* 
			Depth at which the object is fully submerged (at which point the
			force no longer gets stronger the deeper the particle goes). This
			value represents the height below and above the center of the 
			particle up to which the object extends.
		*/
		real maxDepth;

		// Volume of the object submerged
		real volume;

		// The density of the liquid (1000 for water)
		real liquidDensity;

	public:

		// Argumented constructor (by default the liquid is water)
		ParticleBuoyancy(real liquidHeight, real maxDepth, real volume,
			real liquidDensity = 1000.0f) : liquidHeight{liquidHeight},
			maxDepth{ maxDepth }, volume{ volume }, 
			liquidDensity{ liquidDensity } {};

		/*
			Adds the buoyancy force to a particle. The force is calculated
			as such: when the particle's y component is above liquid level,
			there is no force. When the y component is between the liquid
			level and the maxDepth, the force is a spring-like, equal to
			dvp where d is the density, v the volume, and p the proportion
			of the object in the liquid calculated as p = (y0 - y - s) / 2s.
			This formula works best on cubes, but is a good approximation
			for other objects. When fully submerged (y < maxDepth), the
			force is dv, where the proportion is 1.
		*/
		virtual void updateForce(Particle* particle, real duration) const;
	};
}

#endif