
#include "particleGravity.h"

using namespace pe;

void ParticleGravity::updateForce(Particle* particle, real duration) const {
	if (particle->hasFiniteMass()) {
		particle->addForce(gravity * particle->getMass());
	}
}