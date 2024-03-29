// Source file for the particle spring damper class

#include "particleSpringDamper.h"

using namespace pe;

// updateForce definition for the normal spring force class
void ParticleSpringDamper::updateForce(Particle* particle, real duration) {
    Vector3D particleToParticle = otherParticle->position - particle->position;
    real currentLength = particleToParticle.magnitude();
    real deltaX = currentLength - restingLength;
    Vector3D direction;

    if (currentLength == 0) {
        direction = Vector3D(0, 1, 0);
    }
    else {
        direction = particleToParticle.normalized();
    }

    if (currentLength > 1.2 * restingLength) {
        Vector3D center = (otherParticle->position + particle->position) * (1.0/2.0f);
        if (particle->isAwake) {
            particle->position = center - (direction * 0.6f * restingLength);
        }
    }
    else if (currentLength < 0.5 * restingLength) {
        Vector3D center = (otherParticle->position + particle->position) * (1.0/2.0f);
        if (particle->isAwake) {
            particle->position = center - (direction * (restingLength / 4.0f));
        }
    }

    Vector3D vClose = direction * (particleToParticle * direction);
    Vector3D force =  direction * springConstant * deltaX 
        + vClose * dampingConstant;

    particle->addForce(force);
}
