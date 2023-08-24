// Header file for particle drag force class

#ifndef PARTICLE_DRAG_H
#define PARTICLE_DRAG_H

#include "particleForceGenerator.h"

namespace pe {

	/* 
		The drag force is based on the formula drag = 
		-v * (k1 * s + k2 * s ^ 2) where s is the speed (magnitude of 
		velocity) and v is the normalized velocity
	*/
	class ParticleDrag : public ParticleForceGenerator {

	private:
		
		// Particle velocity drag coefficient 
		real k1;
		
		// Particle velocity squared drag coefficient
		real k2;

	public:

		ParticleDrag(real k1, real k2) : k1{ k1 }, k2{ k2 }{}

		virtual void updateForce(Particle* particle, real duration);
	};
}

#endif