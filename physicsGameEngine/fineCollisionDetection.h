
#ifndef FINE_COLLISION_DETECTION_H
#define FINE_COLLISION_DETECTION_H

#include "contact.h"
#include "solidSphere.h"
#include "collisionBox.h"

namespace pe {

    static inline real transformToAxis(
        const Box& box,
        const Vector3D& axis
    ){
        return
            box.halfSize.x * realAbs(axis * box.getAxis(0)) +
            box.halfSize.y * realAbs(axis * box.getAxis(1)) +
            box.halfSize.z * realAbs(axis * box.getAxis(2));
    }


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
    ){
        // Projecting the half-size onto axis
        real oneProject = transformToAxis(one, axis);
        real twoProject = transformToAxis(two, axis);

        // Projecting this onto the axis
        real distance = realAbs(toCentre * axis);

        /*
            Here, the a positive value indicates overlap, 
            and a negative value indicates separation.
        */
        return oneProject + twoProject - distance;
    }

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
    ){
        // Project the half-size of one onto axis
        real oneProject = transformToAxis(one, axis);
        real twoProject = transformToAxis(two, axis);

        // Project this onto the axis
        real distance = realAbs(toCentre.scalarProduct(axis));

        // Check for overlap
        return (distance < oneProject + twoProject);
    }


    bool boxAndBox(
        const Box one,
        const Box& two
    ){
        // Finds the vector between the two centres
        Vector3D toCentre = two.getAxis(3) - one.getAxis(3);

        return (
            // Check on box one's axes first
            overlapOnAxis(one, two, one.getAxis(0), toCentre) &&
            overlapOnAxis(one, two, one.getAxis(1), toCentre) &&
            overlapOnAxis(one, two, one.getAxis(2), toCentre) &&

            // And on two's
            overlapOnAxis(one, two, two.getAxis(0), toCentre) &&
            overlapOnAxis(one, two, two.getAxis(1), toCentre) &&
            overlapOnAxis(one, two, two.getAxis(2), toCentre) &&

            // Now on the cross products
            overlapOnAxis(one, two, one.getAxis(0) % two.getAxis(0), toCentre) &&
            overlapOnAxis(one, two, one.getAxis(0) % two.getAxis(1), toCentre) &&
            overlapOnAxis(one, two, one.getAxis(0) % two.getAxis(2), toCentre) &&
            overlapOnAxis(one, two, one.getAxis(1) % two.getAxis(0), toCentre) &&
            overlapOnAxis(one, two, one.getAxis(1) % two.getAxis(1), toCentre) &&
            overlapOnAxis(one, two, one.getAxis(1) % two.getAxis(2), toCentre) &&
            overlapOnAxis(one, two, one.getAxis(2) % two.getAxis(0), toCentre) &&
            overlapOnAxis(one, two, one.getAxis(2) % two.getAxis(1), toCentre) &&
            overlapOnAxis(one, two, one.getAxis(2) % two.getAxis(2), toCentre)
        );
    }


    static inline bool tryAxis(
        const Box& one,
        const Box& two,
        Vector3D axis,
        const Vector3D& toCentre,
        unsigned index,

        // These values may be updated
        real& smallestPenetration,
        unsigned& smallestCase
    ){
        // Almost parallel axes are not checked
        if (axis.magnitudeSquared() < 0.0001) return true;
        axis.normalize();

        real penetration = penetrationOnAxis(one, two, axis, toCentre);

        if (penetration < 0) return false;
        if (penetration < smallestPenetration) {
            smallestPenetration = penetration;
            smallestCase = index;
        }
        return true;
    }


    void fillPointFaceBoxBox(
        const Box& one,
        const Box& two,
        const Vector3D& toCentre,
        std::vector<Contact>& contacts,
        unsigned best,
        real pen
    )
    {
        /*
            This function is called when we know that a vertex from
            box two is in contact with box one.
        */

        Contact contact;

        Vector3D normal = one.getAxis(best);
        if (one.getAxis(best) * toCentre > 0)
        {
            normal = normal * -1.0f;
        }

        Vector3D vertex = two.halfSize;
        if (two.getAxis(0) * normal < 0) vertex.x = -vertex.x;
        if (two.getAxis(1) * normal < 0) vertex.y = -vertex.y;
        if (two.getAxis(2) * normal < 0) vertex.z = -vertex.z;

        // Contact data
        contact.contactNormal = normal;
        contact.penetration = pen;
        contact.contactPoint = two.transformMatrix * vertex;
        contact.body[0] = one.body;
        contact.body[1] = two.body;

        contacts.push_back(contact);
    }

    static inline Vector3D contactPoint(
        const Vector3D& pOne,
        const Vector3D& dOne,
        real oneSize,
        const Vector3D& pTwo,
        const Vector3D& dTwo,
        real twoSize,
        bool useOne
    ){
        Vector3D toSt, cOne, cTwo;
        real dpStaOne, dpStaTwo, dpOneTwo, smOne, smTwo;
        real denom, mua, mub;

        smOne = dOne.magnitudeSquared();
        smTwo = dTwo.magnitudeSquared();
        dpOneTwo = dTwo * dOne;

        toSt = pOne - pTwo;
        dpStaOne = dOne * toSt;
        dpStaTwo = dTwo * toSt;

        denom = smOne * smTwo - dpOneTwo * dpOneTwo;

        // Parrallel lines
        if (realAbs(denom) < 0.0001f) {
            return useOne ? pOne : pTwo;
        }

        mua = (dpOneTwo * dpStaTwo - smTwo * dpStaOne) / denom;
        mub = (smOne * dpStaTwo - dpOneTwo * dpStaOne) / denom;

        /*
            If either of the edges has the nearest point out
            of bounds, then the edges aren't crossed, and we have
            an edge-face contact. Our point is on the edge.
        */
        if (mua > oneSize ||
            mua < -oneSize ||
            mub > twoSize ||
            mub < -twoSize
        ) {
            return useOne ? pOne : pTwo;
        }
        else
        {
            cOne = pOne + dOne * mua;
            cTwo = pTwo + dTwo * mub;

            return cOne * 0.5 + cTwo * 0.5;
        }
    }


    unsigned int boxAndBox(
        const Box& one,
        const Box& two,
        std::vector<Contact>& contacts
    ){
        Vector3D toCentre = two.getAxis(3) - one.getAxis(3);
        real pen = REAL_MAX;
        unsigned best = 0xffffff;

        if (!tryAxis(one, two, one.getAxis(0), toCentre, 0, pen, best)) return 0;
        if (!tryAxis(one, two, one.getAxis(1), toCentre, 1, pen, best)) return 0;
        if (!tryAxis(one, two, one.getAxis(2), toCentre, 2, pen, best)) return 0;

        if (!tryAxis(one, two, two.getAxis(0), toCentre, 3, pen, best)) return 0;
        if (!tryAxis(one, two, two.getAxis(1), toCentre, 4, pen, best)) return 0;
        if (!tryAxis(one, two, two.getAxis(2), toCentre, 5, pen, best)) return 0;

        unsigned int bestSingleAxis = best;

        if (!tryAxis(one, two, one.getAxis(0) % two.getAxis(0), toCentre, 6, pen, best)) return 0;
        if (!tryAxis(one, two, one.getAxis(0) % two.getAxis(1), toCentre, 7, pen, best)) return 0;
        if (!tryAxis(one, two, one.getAxis(0) % two.getAxis(2), toCentre, 8, pen, best)) return 0;
        if (!tryAxis(one, two, one.getAxis(1) % two.getAxis(0), toCentre, 9, pen, best)) return 0;
        if (!tryAxis(one, two, one.getAxis(1) % two.getAxis(1), toCentre, 10, pen, best)) return 0;
        if (!tryAxis(one, two, one.getAxis(1) % two.getAxis(2), toCentre, 11, pen, best)) return 0;
        if (!tryAxis(one, two, one.getAxis(2) % two.getAxis(0), toCentre, 12, pen, best)) return 0;
        if (!tryAxis(one, two, one.getAxis(2) % two.getAxis(1), toCentre, 13, pen, best)) return 0;
        if (!tryAxis(one, two, one.getAxis(2) % two.getAxis(2), toCentre, 14, pen, best)) return 0;

        assert(best != 0xffffff);

        if (best < 3){
            fillPointFaceBoxBox(one, two, toCentre, contacts, best, pen);
            return 1;
        }
        else if (best < 6) {
            fillPointFaceBoxBox(two, one, toCentre * -1.0f, contacts, best - 3, pen);
            return 1;
        }
        else{
            best -= 6;
            unsigned oneAxisIndex = best / 3;
            unsigned twoAxisIndex = best % 3;
            Vector3D oneAxis = one.getAxis(oneAxisIndex);
            Vector3D twoAxis = two.getAxis(twoAxisIndex);
            Vector3D axis = oneAxis % twoAxis;
            axis.normalize();

            if (axis * toCentre > 0) axis = axis * -1.0f;

            Vector3D ptOnOneEdge = one.halfSize;
            Vector3D ptOnTwoEdge = two.halfSize;
            for (unsigned i = 0; i < 3; i++)
            {
                if (i == oneAxisIndex) ptOnOneEdge[i] = 0;
                else if (one.getAxis(i) * axis > 0) ptOnOneEdge[i] = -ptOnOneEdge[i];

                if (i == twoAxisIndex) ptOnTwoEdge[i] = 0;
                else if (two.getAxis(i) * axis < 0) ptOnTwoEdge[i] = -ptOnTwoEdge[i];
            }

            ptOnOneEdge = one.transformMatrix * ptOnOneEdge;
            ptOnTwoEdge = two.transformMatrix * ptOnTwoEdge;

            Vector3D vertex = contactPoint(
                ptOnOneEdge, oneAxis, one.halfSize[oneAxisIndex],
                ptOnTwoEdge, twoAxis, two.halfSize[twoAxisIndex],
                bestSingleAxis > 2
            );

            // Contact data
            Contact contact;

            contact.penetration = pen;
            contact.contactNormal = axis;
            contact.contactPoint = vertex;
            contact.body[0] = one.body;
            contact.body[1] = two.body;

            contacts.push_back(contact);

            return 1;
        }
        return 0;
    }


    bool sphereAndSphere(
        const Ball& one,
        const Ball& two
    ){
        Vector3D midline = one.getAxis(3) - two.getAxis(3);

        return midline.magnitudeSquared() <
            (one.radius + two.radius) * (one.radius + two.radius);
    }



    unsigned int sphereAndSphere(
        const Ball& one,
        const Ball& two,
        std::vector<Contact>& data
    ){

        Vector3D positionOne = one.getAxis(3);
        Vector3D positionTwo = two.getAxis(3);

        Vector3D midline = positionOne - positionTwo;
        real size = midline.magnitude();

        if (size <= 0.0f || size >= one.radius + two.radius) {
            return 0;
        }

        Vector3D normal = midline * (((real)1.0) / size);

        Contact contact;
        contact.contactNormal = normal;
        contact.contactPoint = positionOne + midline * (real)0.5;
        contact.penetration = (one.radius + two.radius - size);
        contact.body[0] = one.body;
        contact.body[1] = two.body;

        data.push_back(contact);
        return 1;
    }


    unsigned boxAndSphere(
        const Box& box,
        const Ball& sphere,
        std::vector<Contact>& data
    ){
        Vector3D centre = sphere.getAxis(3);
        Vector3D relCentre = box.transformMatrix.inverseTransform(centre);

        if (
            realAbs(relCentre.x) - sphere.radius > box.halfSize.x ||
            realAbs(relCentre.y) - sphere.radius > box.halfSize.y ||
            realAbs(relCentre.z) - sphere.radius > box.halfSize.z
        ){
            return 0;
        }

        Vector3D closestPt(0, 0, 0);
        real dist;

        dist = relCentre.x;
        if (dist > box.halfSize.x) dist = box.halfSize.x;
        if (dist < -box.halfSize.x) dist = -box.halfSize.x;
        closestPt.x = dist;

        dist = relCentre.y;
        if (dist > box.halfSize.y) dist = box.halfSize.y;
        if (dist < -box.halfSize.y) dist = -box.halfSize.y;
        closestPt.y = dist;

        dist = relCentre.z;
        if (dist > box.halfSize.z) dist = box.halfSize.z;
        if (dist < -box.halfSize.z) dist = -box.halfSize.z;
        closestPt.z = dist;

        dist = (closestPt - relCentre).magnitudeSquared();
        if (dist > sphere.radius * sphere.radius) return 0;

        Vector3D closestPtWorld = box.transformMatrix.transform(closestPt);

        Contact contact;
        contact.contactNormal = (closestPtWorld - centre);
        contact.contactNormal.normalize();
        contact.contactPoint = closestPtWorld;
        contact.penetration = sphere.radius - realSqrt(dist);
        contact.body[0] = box.body;
        contact.body[1] = sphere.body;

        data.push_back(contact);
        return 1;
    }


    void generateContactBoxAndBox(
        const Polyhedron& one,
        const Polyhedron& two,
        std::vector<Contact>& contacts,
        real restitution,
        real friction
    ) {
        
        std::vector<Contact> contactsGenerated;

        Box boxOne(one);
        Box boxTwo(two);
        boxAndBox(boxOne, boxTwo, contactsGenerated);

        for (Contact& contact : contactsGenerated) {
            contact.restitution = restitution;
            contact.friction = friction;

            contacts.push_back(contact);
        }
    }


    void generateContactBoxAndSphere(
        const Polyhedron& one,
        const Polyhedron& two,
        std::vector<Contact>& contacts,
        real restitution,
        real friction
    ) {

        std::vector<Contact> contactsGenerated;

        Box box(one);
        Ball sphere(two);
        boxAndSphere(box, sphere, contactsGenerated);

        for (Contact& contact : contactsGenerated) {
            contact.restitution = restitution;
            contact.friction = friction;

            contacts.push_back(contact);
        }
    }


    void generateContactSphereAndSphere(
        const Polyhedron& one,
        const Polyhedron& two,
        std::vector<Contact>& contacts,
        real restitution,
        real friction
    ) {

        std::vector<Contact> contactsGenerated;

        Ball sphereOne(one);
        Ball sphereTwo(two);
        sphereAndSphere(sphereOne, sphereTwo, contactsGenerated);

        for (Contact& contact : contactsGenerated) {
            contact.restitution = restitution;
            contact.friction = friction;

            contacts.push_back(contact);
        }
    }

}

#endif