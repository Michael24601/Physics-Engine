// Source file for the particle force registry class

#include "particleForceRegistry.h"

using namespace pe;

void ParticleForceRegistry::addForce(Particle* particle,
	ParticleForceGenerator* force) {
	registrations.push_back(ParticleForceRegistration{particle, force});
}


void ParticleForceRegistry::removeForce(Particle* particle,
	ParticleForceGenerator* force) {
	// Uses a lmabda to find value to remove
	auto iterator = std::find_if(registrations.begin(), registrations.end(),
		[particle, force](const ParticleForceRegistration& value) -> bool {
				return (value.force == force && value.particle == particle);
		});
	// Remove registration only if found
	if (iterator != registrations.end()) {
		registrations.erase(iterator);
	}
}

void ParticleForceRegistry::removeAll() {
	registrations.clear();
}

// Have all forces in the registry update their particles
void ParticleForceRegistry::updateAll(real duration) {
	for (int i = 0; i < registrations.size(); i++) {
		registrations[i].force->updateForce(registrations[i].particle, 
			duration);
	}
}