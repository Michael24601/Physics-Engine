
#ifndef PARTICLE_COLLISION_DETECTION_H
#define PARTICLE_COLLISION_DETECTION_H

#include "fineCollisionDetection.h"
#include "collisionBox.h"
#include "particleContact.h"

namespace pe {

    /*
        Detects collision between a point and a box.
    */
    unsigned int boxAndPoint(
        const Vector3D& point,
        const Box& box,
        std::vector<ParticleContact>& data
    ) {
        // Transforms the point into box coordinates
        Vector3D relPt = box.body->transformMatrix.inverseTransform(
            point
        );
        Vector3D normal;
        /*
            Check each axis, looking for the axis on which the
            penetration is least deep.
        */
        real min_depth = box.halfSize.x - realAbs(relPt.x);
        if (min_depth < 0) return 0;
        normal = box.getAxis(0) * ((relPt.x < 0) ? -1 : 1);
        real depth = box.halfSize.y - realAbs(relPt.y);
        if (depth < 0) return 0;
        else if (depth < min_depth)
        {
            min_depth = depth;
            normal = box.getAxis(1) * ((relPt.y < 0) ? -1 : 1);
        }
        depth = box.halfSize.z - realAbs(relPt.z);
        if (depth < 0) return 0;
        else if (depth < min_depth)
        {
            min_depth = depth;
            normal = box.getAxis(2) * ((relPt.z < 0) ? -1 : 1);
        }

        ParticleContact contact;
        contact.contactNormal = normal;
        contact.interpenetration = min_depth;

        data.push_back(contact);

        return 1;
    }


    unsigned int sphereAndPoint(
        const Vector3D& point,
        const Sphere& sphere,
        std::vector<ParticleContact>& data
    ) {
        // Transforms the point into sphere coordinates
        Vector3D relPt = sphere.body->transformMatrix.inverseTransform(
            point
        );

        real penetrationDepth = relPt.magnitude() - sphere.radius;

        if (penetrationDepth > 0) {
            return 0;
        }

        ParticleContact contact;
        contact.contactNormal = relPt.normalized();
        contact.interpenetration = -penetrationDepth;
        data.push_back(contact);

        return 1;
    }


    void generateContactParticleAndBox(
        Particle* particle,
        const Polyhedron& one,
        std::vector<ParticleContact>& contacts,
        real restitution
    ) {

        std::vector<ParticleContact> contactsGenerated;

        Box box(one);
        box.halfSize *= 1.2;
        boxAndPoint(particle->position, box, contactsGenerated);

        for (ParticleContact& contact : contactsGenerated) {
            contact.restitutionCoefficient = restitution;
            contact.particle[0] = particle;
            contact.particle[1] = NULL;
            contacts.push_back(contact);
        }
    }


    void generateContactParticleAndSphere(
        Particle* particle,
        const SolidSphere& one,
        std::vector<ParticleContact>& contacts,
        real restitution
    ) {

        std::vector<ParticleContact> contactsGenerated;

        Sphere sphere(one);
        sphere.radius *= 1.2;
        sphereAndPoint(particle->position, sphere, contactsGenerated);

        for (ParticleContact& contact : contactsGenerated) {
            contact.restitutionCoefficient = restitution;
            contact.particle[0] = particle;
            contact.particle[1] = NULL;
            contacts.push_back(contact);
        }
    }
}

#endif