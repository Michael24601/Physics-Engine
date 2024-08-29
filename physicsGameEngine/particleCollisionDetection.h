
#ifndef PARTICLE_COLLISION_DETECTION_H
#define PARTICLE_COLLISION_DETECTION_H

#include "fineCollisionDetection.h"
#include "collisionVolume.h"
#include "particleContact.h"

namespace pe {

    /*
        Detects collision between a point and a box.
    */
    unsigned int boxAndPoint(
        const Vector3D& point,
        const Box& box,
        std::vector<ParticleContact>& data
    );


    unsigned int sphereAndPoint(
        const Vector3D& point,
        const Ball& sphere,
        std::vector<ParticleContact>& data
    );


    void generateContactParticleAndBox(
        Particle* particle,
        const Polyhedron& one,
        std::vector<ParticleContact>& contacts,
        real restitution
    );


    void generateContactParticleAndSphere(
        Particle* particle,
        const SolidSphere& one,
        std::vector<ParticleContact>& contacts,
        real restitution
    );

}

#endif