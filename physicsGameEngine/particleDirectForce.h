
#ifndef PARTICLE_DIRECT_FORCE_H
#define PARTICLE_DIRECT_FORCE_H

#include "particleForceGenerator.h"

namespace pe {

	class ParticleDirectForce : public ParticleForceGenerator {

	public:

		Vector3D force;

		ParticleDirectForce() : force{ Vector3D::ZERO } {}

		ParticleDirectForce(const Vector3D& force) : force{ force } {}

		void updateForce(Particle* particle, real duration) const override {
			if (particle->hasFiniteMass()) {
				particle->addForce(force);
			}
		}

	};
}

#endif