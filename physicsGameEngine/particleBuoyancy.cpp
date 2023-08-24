// Source file for the particle buoyancy force class

#include "particleBuoyancy.h"

using namespace pe;

void ParticleBuoyancy::updateForce(Particle* particle, real duration) {

	real particleDepth = particle->position.y;

	/*
		If object is not submerged (if y is larger than the water height
		and the depth of the object below its center).
	*/
	if (particleDepth >= liquidHeight + maxDepth) return;

	// The buoyancy force is a force acting solely on the y axis (upward) 
	Vector3D force;

	// If fully submerged
	if (particleDepth <= liquidHeight - maxDepth) {
		force.y = liquidDensity * volume;
	}
	// Else if partially submerged
	else {
		real proportion = (particleDepth - maxDepth - liquidHeight) / 
			2 * maxDepth;
		force.y = liquidDensity * volume * proportion;
	}

	particle->addForce(force);

}