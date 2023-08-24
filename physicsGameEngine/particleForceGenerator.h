/*
	Particle force interface that generalizes the way a force affects a
	particle.
*/

#ifndef PARTICLE_FORCE_GENERATOR_H
#define PARTICLE_FORCE_GENERATOR_H

#include "vector3D.h"
#include "particle.h"

namespace pe {

	class ParticleForceGenerator {
	public:

		/*
			Adds a force to a particle by changing the object somehow.
			This force is automatically removed from the accumulator, but
			can then be added again on the next frame. Because the 
			updateForce fucntion can have an effect on the acceleration, 
			based on multiple parameters like velocity, mass, etc... we also 
			include the duration parameter in case the frame size affects 
			how the force affects the particle.
		*/
		virtual void updateForce(Particle* particle, real duration) = 0;
	};
}

#endif