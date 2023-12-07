// Source file for the particle spring force class

#include "particleSpringForce.h"

using namespace pe;

// updateForce definition for the normal spring force class
void ParticleSpringForce::updateForce(Particle* particle, real duration) {
	 // According to the formula defined in the header
    Vector3D force = particle->position;
    force -= otherParticle->position;
    
    real magnitude = force.magnitude();
    real displacement = magnitude - restingLength;

    // Check if the cable is stretched beyond its maximum length
    if (displacement > 0) {
        // Apply the force to bring the particles closer
        force.normalize();
        force *= -springConstant * displacement;
        particle->addForce(force);
    } else {
        // No force is applied if the cable is not stretched
    }
}
