// Source file for the anchored spring force class

#include "anchoredSpringForce.h"

using namespace pe;

void AnchoredSpringForce::updateForce(Particle* particle, real duration) const {
	// According to the formula defined in the header
	Vector3D force = particle->position;
	force -= *anchor;
	real magnitude = force.magnitude();
	magnitude = realAbs(magnitude - restingLength);
	magnitude *= springConstant;
	force.normalize();
	force *= -magnitude;
	particle->addForce(force);
}