// Source file for the particle spring force class

#include "particleSpringForce.h"

using namespace pe;

// updateForce definition for the normal spring force class
void ParticleSpringForce::updateForce(Particle* particle, real duration) const {
	// According to the formula defined in the header
	Vector3D force = particle->position;
	force -= otherParticle->position;
	real magnitude = force.magnitude();
	magnitude = realAbs(magnitude - restingLength);
	magnitude *= springConstant;
	force.normalize();
	force *= -magnitude;
	particle->addForce(force);
}
