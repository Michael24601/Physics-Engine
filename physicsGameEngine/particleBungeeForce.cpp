// Source file for the particle bungee force class

#include "particleBungeeForce.h"

using namespace pe;

// updateForce definition for the normal spring force class
void ParticleBungeeForce::updateForce(Particle* particle, real duration) {
	// According to the formula defined in the header
	Vector3D force = particle->position;
	force -= otherParticle->position;
	real magnitude = force.magnitude();
	magnitude = realAbs(magnitude - restingLength);

	// As stated, the bungee rope only exerts a force when elongated
	if (magnitude > restingLength) {
		magnitude *= springConstant;
		force.normalize();
		force *= -magnitude;
		particle->addForce(force);
	}
}
