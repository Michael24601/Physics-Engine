
#include "particleGravity.h"

using namespace pe;

void ParticleGravity::updateForce(Particle* particle, real duration) {
	if (particle->hasFiniteMass()) {
		particle->addForce(gravity * particle->getMass());
	}
}