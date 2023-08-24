// Source file for particle drag force class

#include "particleDrag.h"

using namespace pe;

void ParticleDrag::updateForce(Particle* particle, real duration) {
	// Uses the formula defined in the header class file
	Vector3D force = particle->velocity;
	real dragCoefficient = force.magnitude();
	dragCoefficient = (k1 * dragCoefficient) 
		+ (k2 * dragCoefficient * dragCoefficient);
	force.normalize();
	force *= -dragCoefficient;
	particle->addForce(force);
}