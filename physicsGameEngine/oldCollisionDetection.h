
/*
#ifndef OLD_COLLISION_DETECTION_H
#define OLD_COLLISION_DETECTION_H

#include "rectangularPrism.h"
#include "contact.h"
#include <vector>

namespace pe {

    real transformToAxis(
        const RectangularPrism& box,
        const Vector3D& axis
    ) {
        return box.halfSize.x * realAbs(axis.scalarProduct(box.getAxis(0))) +
            box.halfSize.y * realAbs(axis.scalarProduct(box.getAxis(1))) +
            box.halfSize.z * realAbs(axis.scalarProduct(box.getAxis(2)));
    }

    bool overlapOnAxis(
        const RectangularPrism& one,
        const RectangularPrism& two,
        const Vector3D& axis
    ) {
        // Project the half-size of one onto axis.
        real oneProject = transformToAxis(one, axis);
        real twoProject = transformToAxis(two, axis);
        // Find the vector between the two centers.
        Vector3D toCenter = two.getAxis(3) - one.getAxis(3);
        // Project this onto the axis.
        real distance = realAbs(toCenter.scalarProduct(axis));
        // Check for overlap.
        return (distance < oneProject + twoProject);
    }


    unsigned boxAndPoint(
        const RectangularPrism& box,
        const Vector3D& point,
        const RectangularPrism& otherBox,
        std::vector<Contact>& data
    ) {
        // Transform the point into box coordinates.
        Vector3D relPt = box.body->transformMatrix.inverseTransform(point);
        Vector3D normal;
        // Check each axis, looking for the axis on which the
        // penetration is least deep.
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
        // Compile the contact.
        Contact contact;
        contact.contactNormal = normal;
        contact.contactPoint = point;
        contact.penetration = min_depth;
        // Write the appropriate data.
        contact.body[0] = otherBox.body;
        // Note that we don’t know what rigid body the point
        // belongs to, so we just use NULL. Where this is called
        // this value can be left, or filled in.
        contact.body[1] = box.body;

        data.push_back(contact);

        return 1;
    }


    unsigned int edgeAndEdge(
        const RectangularPrism& one,
        const RectangularPrism& two,
        std::vector<Contact> data
    ) {
        // Complete
    }



    int pe::returnContactsBox(
        const RectangularPrism& p1,
        const RectangularPrism& p2,
        std::vector<Contact>& contactsToBeResolved
    ) {

        std::vector<Contact> contacts;

        for (const auto& vertexA : p1.globalVertices) {
            boxAndPoint(p2, vertexA, p1, contacts);
        }
        for (const auto& vertexA : p2.globalVertices) {
            boxAndPoint(p1, vertexA, p2, contacts);
        }

        // Here check edgeAndEdge

        if (contacts.size() == 0) {
            return 0;
        }

        Contact* maxContact = &contacts[0];
        for (int i = 1; i < contacts.size(); i++) {
            if (maxContact->penetration < contacts[i].penetration) {
                maxContact = &contacts[i];
            }
        }
        contactsToBeResolved.push_back(*maxContact);

        // Return the number of contacts found (usually 0 or 1)
        return 1;
    }

}

#endif
*/