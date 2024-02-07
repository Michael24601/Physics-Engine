
// ParticleAngleConstraint.h
#ifndef PARTICLE_ANGLE_CONSTRAINT_H
#define PARTICLE_ANGLE_CONSTRAINT_H

#include "particle.h"

namespace pe {

    class ParticleAngleConstraint {

    public:

        ParticleAngleConstraint(
            Particle* p1, 
            Particle* p2, 
            Particle* p3, 
            real restAngle,
            real stiffness
        ) : particle1(p1), particle2(p2), particle3(p3), 
            restAngle(restAngle), stifness{stifness} {}

        void applyConstraint() {
            // Calculate the current angle between the particles
            Vector3D v1 = particle1->position - particle2->position;
            Vector3D v2 = particle3->position - particle2->position;
            real currentAngle = std::acos(
                v1.scalarProduct(v2) / (v1.magnitude() * v2.magnitude())
            );

            // Calculate the angle difference
            real angleDiff = currentAngle - restAngle;

            // Calculate the corrective force based on the angle difference
            Vector3D force1 = calculateForce(
                particle1, particle2, particle3, angleDiff, stifness
            );
            Vector3D force2 = calculateForce(
                particle2, particle1, particle3, angleDiff, stifness
            );
            Vector3D force3 = calculateForce(
                particle3, particle2, particle1, angleDiff, stifness
            );

            // Apply forces to particles
            particle1->addForce(force1);
            particle2->addForce(force2);
            particle3->addForce(force3);
        }

    private:
        Particle* particle1;
        Particle* particle2;
        Particle* particle3;
        real restAngle;
        real stifness;

        Vector3D calculateForce(
            Particle* p1, 
            Particle* p2, 
            Particle* p3, 
            real angleDiff,
            real stifndess
        ) {

            // Assuming linear spring force for simplicity
            Vector3D v1 = p1->position - p2->position;
            Vector3D v2 = p3->position - p2->position;
            real len1 = v1.magnitude();
            real len2 = v2.magnitude();

            // Calculate the direction of the force
            Vector3D forceDirection = 
                (v1 * (1/(real)len1)) - (v2 * (1 / (real)len2));
            forceDirection.normalize();

            // k is a stiffness constant
            real forceMagnitude = stifness * angleDiff;

            // Return the force to be applied to the particle
            return (forceDirection * forceMagnitude);
        }
    };
}

#endif
