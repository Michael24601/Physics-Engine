

#include "contact.h"
#include "rectangularPrism.h"

namespace pe {

    static inline real transformToAxis2(
        const RectangularPrism& box,
        const Vector3D& axis
    )
    {
        return
            box.halfSize.x * realAbs(axis * box.getAxis(0)) +
            box.halfSize.y * realAbs(axis * box.getAxis(1)) +
            box.halfSize.z * realAbs(axis * box.getAxis(2));
    }


    /*
 * This function checks if the two boxes overlap
 * along the given axis, returning the ammount of overlap.
 * The final parameter toCentre
 * is used to pass in the vector between the boxes centre
 * points, to avoid having to recalculate it each time.
 */
    static inline real penetrationOnAxis2(
        const RectangularPrism& one,
        const RectangularPrism& two,
        const Vector3D& axis,
        const Vector3D& toCentre
    )
    {
        // Project the half-size of one onto axis
        real oneProject = transformToAxis2(one, axis);
        real twoProject = transformToAxis2(two, axis);

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
    static inline bool overlapOnAxis2(
        const RectangularPrism& one,
        const RectangularPrism& two,
        const Vector3D& axis,
        const Vector3D& toCentre
    )
    {
        // Project the half-size of one onto axis
        real oneProject = transformToAxis2(one, axis);
        real twoProject = transformToAxis2(two, axis);

        // Project this onto the axis
        real distance = realAbs(toCentre.scalarProduct(axis));

        // Check for overlap
        return (distance < oneProject + twoProject);
    }

    // This preprocessor definition is only used as a convenience
    // in the boxAndBox intersection  method.
    #define TEST_OVERLAP(axis) overlapOnAxis2(one, two, (axis), toCentre)

    bool boxAndBox(
        const RectangularPrism& one,
        const RectangularPrism& two
    )
    {
        // Find the vector between the two centres
        Vector3D toCentre = two.getAxis(3) - one.getAxis(3);

        return (
            // Check on box one's axes first
            TEST_OVERLAP(one.getAxis(0)) &&
            TEST_OVERLAP(one.getAxis(1)) &&
            TEST_OVERLAP(one.getAxis(2)) &&

            // And on two's
            TEST_OVERLAP(two.getAxis(0)) &&
            TEST_OVERLAP(two.getAxis(1)) &&
            TEST_OVERLAP(two.getAxis(2)) &&

            // Now on the cross products
            TEST_OVERLAP(one.getAxis(0) % two.getAxis(0)) &&
            TEST_OVERLAP(one.getAxis(0) % two.getAxis(1)) &&
            TEST_OVERLAP(one.getAxis(0) % two.getAxis(2)) &&
            TEST_OVERLAP(one.getAxis(1) % two.getAxis(0)) &&
            TEST_OVERLAP(one.getAxis(1) % two.getAxis(1)) &&
            TEST_OVERLAP(one.getAxis(1) % two.getAxis(2)) &&
            TEST_OVERLAP(one.getAxis(2) % two.getAxis(0)) &&
            TEST_OVERLAP(one.getAxis(2) % two.getAxis(1)) &&
            TEST_OVERLAP(one.getAxis(2) % two.getAxis(2))
            );
    }
    #undef TEST_OVERLAP


    static inline bool tryAxis(
        const RectangularPrism& one,
        const RectangularPrism& two,
        Vector3D axis,
        const Vector3D& toCentre,
        unsigned index,

        // These values may be updated
        real& smallestPenetration,
        unsigned& smallestCase
    )
    {
        // Make sure we have a normalized axis, and don't check almost parallel axes
        if (axis.magnitudeSquared() < 0.0001) return true;
        axis.normalize();

        real penetration = penetrationOnAxis2(one, two, axis, toCentre);

        if (penetration < 0) return false;
        if (penetration < smallestPenetration) {
            smallestPenetration = penetration;
            smallestCase = index;
        }
        return true;
    }

    void fillPointFaceBoxBox(
        const RectangularPrism& one,
        const RectangularPrism& two,
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

    // This preprocessor definition is only used as a convenience
    // in the boxAndBox contact generation method.
#define CHECK_OVERLAP(axis, index) \
    if (!tryAxis(one, two, (axis), toCentre, (index), pen, best)) return 0;

    unsigned boxAndBox(
        const RectangularPrism& one,
        const RectangularPrism& two,
        std::vector<Contact>& contacts
    )
    {
        //if (!IntersectionTests::boxAndBox(one, two)) return 0;

        // Find the vector between the two centres
        Vector3D toCentre = two.getAxis(3) - one.getAxis(3);

        // We start assuming there is no contact
        real pen = REAL_MAX;
        unsigned best = 0xffffff;

        // Now we check each axes, returning if it gives us
        // a separating axis, and keeping track of the axis with
        // the smallest penetration otherwise.
        CHECK_OVERLAP(one.getAxis(0), 0);
        CHECK_OVERLAP(one.getAxis(1), 1);
        CHECK_OVERLAP(one.getAxis(2), 2);

        CHECK_OVERLAP(two.getAxis(0), 3);
        CHECK_OVERLAP(two.getAxis(1), 4);
        CHECK_OVERLAP(two.getAxis(2), 5);

        // Store the best axis-major, in case we run into almost
        // parallel edge collisions later
        unsigned bestSingleAxis = best;

        CHECK_OVERLAP(one.getAxis(0) % two.getAxis(0), 6);
        CHECK_OVERLAP(one.getAxis(0) % two.getAxis(1), 7);
        CHECK_OVERLAP(one.getAxis(0) % two.getAxis(2), 8);
        CHECK_OVERLAP(one.getAxis(1) % two.getAxis(0), 9);
        CHECK_OVERLAP(one.getAxis(1) % two.getAxis(1), 10);
        CHECK_OVERLAP(one.getAxis(1) % two.getAxis(2), 11);
        CHECK_OVERLAP(one.getAxis(2) % two.getAxis(0), 12);
        CHECK_OVERLAP(one.getAxis(2) % two.getAxis(1), 13);
        CHECK_OVERLAP(one.getAxis(2) % two.getAxis(2), 14);

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
#undef CHECK_OVERLAP

    void generateContactBoxAndBox(
        const RectangularPrism& one,
        const RectangularPrism& two,
        std::vector<Contact>& contacts,
        real restitution,
        real friction
    ) {
        
        std::vector<Contact> contactsGenerated;
        boxAndBox(one, two, contactsGenerated);

        for (Contact& contact : contactsGenerated) {
            contact.restitution = restitution;
            contact.friction = friction;

            contacts.push_back(contact);
        }

    }

}