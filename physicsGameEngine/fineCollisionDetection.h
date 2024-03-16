
#ifndef FINE_COLLISION_DETECTION_H
#define FINE_COLLISION_DETECTION_H

#include "contact.h"
#include "rectangularPrism.h"
#include "solidSphere.h"

namespace pe {

    /*
        An abstraction used in all of this collision detection system,
        so that any shape (not necessarily a box shape) can use the
        box collision detection and contact generation functions if
        we allow for some small inaccuracies.
        Any polyhedron, so long that it provides a halfSize (dimensions
        for the box) can be considered a box collision-wise,
        with the halfSize as the side length, and the centre of gravity
        of the box as the centre of gravity of the object (they both
        share the same Body object, and by extension the transform matrix).
        Only RectangularPrism will perfectly fit in a box however.
    */
    struct Box {

        // No arg constructor
        Box(RigidBody* body, Vector3D halfSize) :
            body{ body }, halfSize{halfSize} {}

        /*
            Constructs a box from a rectangular prism(They share a shape
            so it is easy).
        */
        Box(const RectangularPrism& prism) {
            body = prism.body;
            halfSize = Vector3D(
                prism.width / 2.0, 
                prism.height / 2.0, 
                prism.depth / 2.0
            );
        }

        RigidBody* body;
        Vector3D halfSize;

        Vector3D getAxis(int index) const {
            return body->transformMatrix.getColumnVector(index);
        }
    };


    /*
        Similarly, this is an abstraction for all the sphere collision
        detection and contact generation functions.
        Only a radius needs to be provided, and any Polyhedron's collisions
        can be simulated as a sphere's with that radius and the centre at
        the centre of gravity of the Polyhedron (they both share the same
        Body object, and by extension the transform matrix).
        Only the SolidSphere object will perfectly fit in a sphere however.
   */
    struct Sphere {

        // No arg constructor
        Sphere(RigidBody* body, real radius) :
            body{ body }, radius{ radius } {}

        /*
            Constructs a box from a rectangular prism(They share a shape
            so it is easy).
        */
        Sphere(const SolidSphere& sphere) {
            body = sphere.body;
            radius = sphere.radius;
        }

        RigidBody* body;
        real radius;

        Vector3D getAxis(int index) const {
            return body->transformMatrix.getColumnVector(index);
        }
    };


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
        // Project the half-size of one onto axis
        real oneProject = transformToAxis(one, axis);
        real twoProject = transformToAxis(two, axis);

        // Project this onto the axis
        real distance = realAbs(toCentre * axis);

        // Return the overlap (i.e. positive indicates
        // overlap, negative indicates separation).
        return oneProject + twoProject - distance;
    }

    /**
     * This function checks if the two boxes overlap
     * along the given axis. The final parameter toCentre
     * is used to pass in the vector between the boxes centre
     * points, to avoid having to recalculate it each time.
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
        // Find the vector between the two centres
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
        // Make sure we have a normalized axis, and don't check almost parallel axes
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
        // This method is called when we know that a vertex from
        // box two is in contact with box one.

        Contact contact;

        // We know which axis the collision is on (i.e. best),
        // but we need to work out which of the two faces on
        // this axis.
        Vector3D normal = one.getAxis(best);
        if (one.getAxis(best) * toCentre > 0)
        {
            normal = normal * -1.0f;
        }

        // Work out which vertex of box two we're colliding with.
        // Using toCentre doesn't work!
        Vector3D vertex = two.halfSize;
        if (two.getAxis(0) * normal < 0) vertex.x = -vertex.x;
        if (two.getAxis(1) * normal < 0) vertex.y = -vertex.y;
        if (two.getAxis(2) * normal < 0) vertex.z = -vertex.z;

        // Create the contact data
        contact.contactNormal = normal;
        contact.penetration = pen;
        contact.contactPoint = two.body->transformMatrix * vertex;
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

        // If this is true, and the contact point is outside
        // the edge (in the case of an edge-face contact) then
        // we use one's midpoint, otherwise we use two's.
        bool useOne)
    {
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

        // Zero denominator indicates parrallel lines
        if (realAbs(denom) < 0.0001f) {
            return useOne ? pOne : pTwo;
        }

        mua = (dpOneTwo * dpStaTwo - smTwo * dpStaOne) / denom;
        mub = (smOne * dpStaTwo - dpOneTwo * dpStaOne) / denom;

        // If either of the edges has the nearest point out
        // of bounds, then the edges aren't crossed, we have
        // an edge-face contact. Our point is on the edge, which
        // we know from the useOne parameter.
        if (mua > oneSize ||
            mua < -oneSize ||
            mub > twoSize ||
            mub < -twoSize)
        {
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
        //if (!IntersectionTests::boxAndBox(one, two)) return 0;

        // Find the vector between the two centres
        Vector3D toCentre = two.getAxis(3) - one.getAxis(3);

        // We start assuming there is no contact
        real pen = REAL_MAX;
        unsigned best = 0xffffff;

        // Now we check each axes, returning if it gives us
        // a separating axis, and keeping track of the axis with
        // the smallest penetration otherwise.
        if (!tryAxis(one, two, one.getAxis(0), toCentre, 0, pen, best)) return 0;
        if (!tryAxis(one, two, one.getAxis(1), toCentre, 1, pen, best)) return 0;
        if (!tryAxis(one, two, one.getAxis(2), toCentre, 2, pen, best)) return 0;

        if (!tryAxis(one, two, two.getAxis(0), toCentre, 3, pen, best)) return 0;
        if (!tryAxis(one, two, two.getAxis(1), toCentre, 4, pen, best)) return 0;
        if (!tryAxis(one, two, two.getAxis(2), toCentre, 5, pen, best)) return 0;

        // Store the best axis-major, in case we run into almost
        // parallel edge collisions later
        unsigned bestSingleAxis = best;

        if (!tryAxis(one, two, one.getAxis(0) % two.getAxis(0), toCentre, 6, pen, best)) return 0;
        if (!tryAxis(one, two, one.getAxis(0) % two.getAxis(1), toCentre, 7, pen, best)) return 0;
        if (!tryAxis(one, two, one.getAxis(0) % two.getAxis(2), toCentre, 8, pen, best)) return 0;
        if (!tryAxis(one, two, one.getAxis(1) % two.getAxis(0), toCentre, 9, pen, best)) return 0;
        if (!tryAxis(one, two, one.getAxis(1) % two.getAxis(1), toCentre, 10, pen, best)) return 0;
        if (!tryAxis(one, two, one.getAxis(1) % two.getAxis(2), toCentre, 11, pen, best)) return 0;
        if (!tryAxis(one, two, one.getAxis(2) % two.getAxis(0), toCentre, 12, pen, best)) return 0;
        if (!tryAxis(one, two, one.getAxis(2) % two.getAxis(1), toCentre, 13, pen, best)) return 0;
        if (!tryAxis(one, two, one.getAxis(2) % two.getAxis(2), toCentre, 14, pen, best)) return 0;

        // Make sure we've got a result.
        assert(best != 0xffffff);

        // We now know there's a collision, and we know which
        // of the axes gave the smallest penetration. We now
        // can deal with it in different ways depending on
        // the case.
        if (best < 3)
        {
            // We've got a vertex of box two on a face of box one.
            fillPointFaceBoxBox(one, two, toCentre, contacts, best, pen);
            return 1;
        }
        else if (best < 6)
        {
            // We've got a vertex of box one on a face of box two.
            // We use the same algorithm as above, but swap around
            // one and two (and therefore also the vector between their
            // centres).
            fillPointFaceBoxBox(two, one, toCentre * -1.0f, contacts, best - 3, pen);
            return 1;
        }
        else
        {
            // We've got an edge-edge contact. Find out which axes
            best -= 6;
            unsigned oneAxisIndex = best / 3;
            unsigned twoAxisIndex = best % 3;
            Vector3D oneAxis = one.getAxis(oneAxisIndex);
            Vector3D twoAxis = two.getAxis(twoAxisIndex);
            Vector3D axis = oneAxis % twoAxis;
            axis.normalize();

            // The axis should point from box one to box two.
            if (axis * toCentre > 0) axis = axis * -1.0f;

            // We have the axes, but not the edges: each axis has 4 edges parallel
            // to it, we need to find which of the 4 for each object. We do
            // that by finding the point in the centre of the edge. We know
            // its component in the direction of the box's collision axis is zero
            // (its a mid-point) and we determine which of the extremes in each
            // of the other axes is closest.
            Vector3D ptOnOneEdge = one.halfSize;
            Vector3D ptOnTwoEdge = two.halfSize;
            for (unsigned i = 0; i < 3; i++)
            {
                if (i == oneAxisIndex) ptOnOneEdge[i] = 0;
                else if (one.getAxis(i) * axis > 0) ptOnOneEdge[i] = -ptOnOneEdge[i];

                if (i == twoAxisIndex) ptOnTwoEdge[i] = 0;
                else if (two.getAxis(i) * axis < 0) ptOnTwoEdge[i] = -ptOnTwoEdge[i];
            }

            // Move them into world coordinates (they are already oriented
            // correctly, since they have been derived from the axes).
            ptOnOneEdge = one.body->transformMatrix * ptOnOneEdge;
            ptOnTwoEdge = two.body->transformMatrix * ptOnTwoEdge;

            // So we have a point and a direction for the colliding edges.
            // We need to find out point of closest approach of the two
            // line-segments.
            Vector3D vertex = contactPoint(
                ptOnOneEdge, oneAxis, one.halfSize[oneAxisIndex],
                ptOnTwoEdge, twoAxis, two.halfSize[twoAxisIndex],
                bestSingleAxis > 2
            );

            // We can fill the contact.
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
        const Sphere& one,
        const Sphere& two
    ){
        // Find the vector between the objects
        Vector3D midline = one.getAxis(3) - two.getAxis(3);

        // See if it is large enough.
        return midline.magnitudeSquared() <
            (one.radius + two.radius) * (one.radius + two.radius);
    }



    unsigned int sphereAndSphere(
        const Sphere& one,
        const Sphere& two,
        std::vector<Contact>& data
    ){

        // Cache the sphere positions
        Vector3D positionOne = one.getAxis(3);
        Vector3D positionTwo = two.getAxis(3);

        // Find the vector between the objects
        Vector3D midline = positionOne - positionTwo;
        real size = midline.magnitude();

        // See if it is large enough.
        if (size <= 0.0f || size >= one.radius + two.radius)
        {
            return 0;
        }

        // We manually create the normal, because we have the
        // size to hand.
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
        const Sphere& sphere,
        std::vector<Contact>& data
    ){
        // Transform the centre of the sphere into box coordinates
        Vector3D centre = sphere.getAxis(3);
        Vector3D relCentre = box.body->transformMatrix.inverseTransform(centre);

        // Early out check to see if we can exclude the contact
        if (realAbs(relCentre.x) - sphere.radius > box.halfSize.x ||
            realAbs(relCentre.y) - sphere.radius > box.halfSize.y ||
            realAbs(relCentre.z) - sphere.radius > box.halfSize.z)
        {
            return 0;
        }

        Vector3D closestPt(0, 0, 0);
        real dist;

        // Clamp each coordinate to the box.
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

        // Check we're in contact
        dist = (closestPt - relCentre).magnitudeSquared();
        if (dist > sphere.radius * sphere.radius) return 0;

        // Compile the contact
        Vector3D closestPtWorld = box.body->transformMatrix.transform(closestPt);

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
        const RectangularPrism& one,
        const RectangularPrism& two,
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
        const RectangularPrism& one,
        const SolidSphere& two,
        std::vector<Contact>& contacts,
        real restitution,
        real friction
    ) {

        std::vector<Contact> contactsGenerated;

        Box box(one);
        Sphere sphere(two);
        boxAndSphere(box, sphere, contactsGenerated);

        for (Contact& contact : contactsGenerated) {
            contact.restitution = restitution;
            contact.friction = friction;

            contacts.push_back(contact);
        }
    }


    void generateContactSphereAndSphere(
        const SolidSphere& one,
        const SolidSphere& two,
        std::vector<Contact>& contacts,
        real restitution,
        real friction
    ) {

        std::vector<Contact> contactsGenerated;

        Sphere sphereOne(one);
        Sphere sphereTwo(two);
        sphereAndSphere(sphereOne, sphereTwo, contactsGenerated);

        for (Contact& contact : contactsGenerated) {
            contact.restitution = restitution;
            contact.friction = friction;

            contacts.push_back(contact);
        }

    }

}

#endif