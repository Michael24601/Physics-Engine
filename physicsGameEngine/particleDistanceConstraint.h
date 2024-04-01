
// ParticleDistanceConstraint.h
#ifndef PARTICLE_DISTANCE_CONSTRAINT_H
#define PARTICLE_DISTANCE_CONSTRAINT_H

#include "particle.h"

namespace pe {

    class ParticleDistanceConstraint {
    private:
        Particle* particle1;
        Particle* particle2;
        real restLength;

    public:
        ParticleDistanceConstraint(Particle* p1, Particle* p2, real restLen)
            : particle1(p1), particle2(p2), restLength(restLen) {}

        void applyConstraint() {
            Vector3D delta = particle2->position - particle1->position;
            real currentDistance = delta.magnitude();
            real offset = (currentDistance - restLength) / (2 * currentDistance);
            if (particle1->isAwake) {
                particle1->position += delta * offset * 0;
            }
            if (particle2->isAwake) {
                particle2->position += delta * -offset * 0;
            }
        }
    };
}

#endif
