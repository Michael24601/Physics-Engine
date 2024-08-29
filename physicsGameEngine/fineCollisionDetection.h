
#ifndef FINE_COLLISION_DETECTION_H
#define FINE_COLLISION_DETECTION_H

#include "contact.h"
#include "solidSphere.h"
#include "collisionVolume.h"

namespace pe {

    static inline real transformToAxis(
        const Box& box,
        const Vector3D& axis
    );


    /*
        This function checks if the two boxes overlap
        along the given axis, returning the ammount of overlap.
        The final parameter toCentre
        is used to pass in the vector between the boxes centre
        points, to avoid having to recalculate it each time.
    */
    static inline real penetrationOnAxis(
        const Box& one,
        const Box& two,
        const Vector3D& axis,
        const Vector3D& toCentre
    );


    /*
        This function checks if the two boxes overlap
        along the given axis. The final parameter toCentre
        is used to pass in the vector between the boxes centre
        points, to avoid having to recalculate it each time.
    */
    static inline bool overlapOnAxis(
        const Box& one,
        const Box& two,
        const Vector3D& axis,
        const Vector3D& toCentre
    );


    bool boxAndBox(
        const Box one,
        const Box& two
    );


    static inline bool tryAxis(
        const Box& one,
        const Box& two,
        Vector3D axis,
        const Vector3D& toCentre,
        unsigned int index,
        // These values may be updated
        real& smallestPenetration,
        unsigned int& smallestCase
    );


    void fillPointFaceBoxBox(
        const Box& one,
        const Box& two,
        const Vector3D& toCentre,
        std::vector<Contact>& contacts,
        unsigned best,
        real pen
    );


    static inline Vector3D contactPoint(
        const Vector3D& pOne,
        const Vector3D& dOne,
        real oneSize,
        const Vector3D& pTwo,
        const Vector3D& dTwo,
        real twoSize,
        bool useOne
    );


    unsigned int boxAndBox(
        const Box& one,
        const Box& two,
        std::vector<Contact>& contacts
    );


    bool sphereAndSphere(
        const Ball& one,
        const Ball& two
    );


    unsigned int sphereAndSphere(
        const Ball& one,
        const Ball& two,
        std::vector<Contact>& data
    );


    unsigned int boxAndSphere(
        const Box& box,
        const Ball& sphere,
        std::vector<Contact>& data
    );


    void generateContactBoxAndBox(
        const Cuboidal& one,
        RigidBody* body1,
        const Cuboidal& two,
        RigidBody* body2,
        std::vector<Contact>& contacts,
        real restitution,
        real friction
    );


    void generateContactBoxAndSphere(
        const Cuboidal& one,
        RigidBody* body1,
        const Spherical& two,
        RigidBody* body2,
        std::vector<Contact>& contacts,
        real restitution,
        real friction
    );


    void generateContactSphereAndSphere(
        const Spherical& one,
        RigidBody* body1,
        const Spherical& two,
        RigidBody* body2,
        std::vector<Contact>& contacts,
        real restitution,
        real friction
    );

}

#endif