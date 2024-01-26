
// ParticleAngleConstraint.h
#ifndef PARTICLE_ANGLE_CONSTRAINT_H
#define PARTICLE_ANGLE_CONSTRAINT_H

#include "particle.h"

namespace pe {

    class ParticleAngleConstraint {
    private:
        Particle* particle1;
        Particle* particle2;
        Particle* particle3;
        real restAngle;

        // Coefficient for the constraint strength, should be between 0 and 1
        real scalingFactor;

    public:
        ParticleAngleConstraint(
            Particle* p1, 
            Particle* p2, 
            Particle* p3, 
            real angle,
            real scalingFactor
        ) : particle1(p1), particle2(p2), particle3(p3), restAngle(angle),
            scalingFactor{scalingFactor} {}

        void applyConstraint() {
            Vector3D v1 = particle1->position - particle2->position;
            Vector3D v3 = particle3->position - particle2->position;

            Vector3D v1Normal = v1;
            Vector3D v3Normal = v3;
            v1Normal.normalize();
            v3Normal.normalize();

            real currentAngle = acos(v1Normal.scalarProduct(v3Normal));
            real offset = tan(currentAngle / 2.0) * sin(currentAngle);

            if (particle1->isAwake) {
                particle1->position += v1.vectorProduct(v3) * -offset 
                    * scalingFactor;
            }
            if (particle3->isAwake) {
                particle3->position += v1.vectorProduct(v3) * offset 
                    * scalingFactor;
            }
        }
    };
}

#endif
