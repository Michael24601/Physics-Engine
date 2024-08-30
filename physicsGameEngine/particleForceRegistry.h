// Registry for keeping track of active forces on particles

#ifndef PARTICLE_FORCE_REGISTRY_H
#define PARTICLE_FORCE_REGISTRY_H

#include <vector>
#include <algorithm>
#include "particleForceGenerator.h"
#include "particle.h"
#include "accuracy.h"

namespace pe {

	class ParticleForceRegistry {

	private:

		struct ParticleForceRegistration
		{
			Particle* particle;
			ParticleForceGenerator* force;
		};

		// List of forces
		typedef std::vector<ParticleForceRegistration> Registry;
		Registry registrations;

	public:

		// Register a force in the registry
		void addForce(Particle* particle, ParticleForceGenerator* force);

		// Remove a force from the registry if it exists
		void removeForce(Particle* particle, ParticleForceGenerator* force);

		// Remove all forces from the registry
		void removeAll();

		/*
			Have all forces in the registry update their particles by being
			added to them.
		*/
		void updateAll(real duration);
	};
}

#endif