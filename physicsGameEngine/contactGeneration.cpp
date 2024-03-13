
#include "contactGeneration.h"

using namespace pe;


unsigned int pe::pointAndConvexPolyhedron(
    const Polyhedron& polyhedron,
    const Vector3D& point,
    const Polyhedron& secondPolyhedron,
    std::vector<Contact>& data) {

    /*
        We can either transform the point into the local basis of the
        polyhedron using its inverse transform matrix, or we can keep
        it as is, but use the normal vectors and vertices of the
        polyhedron in global coordinates.

        Here, we choose to keep it in global coordinates, as that is
        the faster way.
    */
    Vector3D localPoint = point;

    // Initialize variables to track the closest face
    Vector3D closestFaceNormal;
    // Initialize minDepth to a value larger than REAL_MAX
    real minDepth = std::numeric_limits<real>::max();

    // Iterate through each face of the convex polyhedron
    for (size_t i = 0; i < polyhedron.faces.size(); i++) {

        /*
            In accordance with our decision, we get the face normal and
            any point on the face in global space.
        */
        Vector3D faceNormal = polyhedron.faces[i]->getNormal();
        Vector3D pointOnFace = polyhedron.faces[i]->getVertex(0);

        // Compute the vector from the point to the point on the face
        Vector3D toPointOnFace = pointOnFace - localPoint;

        // Compute the penetration depth along the face normal
        real depth = faceNormal.scalarProduct(toPointOnFace);

        // Check if the point is inside or in front of the face
        if (depth < 0) {
            /*
                The point is in front of the face, meaning it's on the
                outside of the polyhedron.
            */
            return 0;
        }

        /*
            If the depth is smaller than the current minimum, update the
            minimum depth and closest face normal.
        */
        if (depth < minDepth) {
            minDepth = depth;
            closestFaceNormal = faceNormal;
        }
    }

    Contact contact;
    /*
        The contact normal is the face normal in world coordinates.
        So if we did our calculations in local coordinates (which we
        didn't for efficiency) we'd have to use th inverse transform
        matrix here.
    */
    contact.contactNormal = closestFaceNormal;
    contact.contactPoint = point;
    // Negative depth since the point is inside the polyhedron
    contact.penetration = -minDepth;
    /*
        We want the body[0] to be the one in whose direction the contact
        normal is, and the other having the opposite contact normal.
        So body[0] is the one to whom the point belongs, and body[1] is the
        primitive sent to this function.
    */
    contact.body[0] = secondPolyhedron.body;
    contact.body[1] = polyhedron.body;

    data.push_back(contact);

    return 1;
}


unsigned int pe::edgeToEdge(
    const Edge& edgeA,
    const Edge& edgeB,
    const Polyhedron& p1,
    const Polyhedron& p2,
    std::vector<Contact>& data) {

    // Initialize variables to track the shallowest penetration
    real minPenetration = std::numeric_limits<real>::max();
    // And corresponding edge pair
    Contact contact;

    // Get the two vertices of the current edges
    Vector3D a0 = edgeA.getVertex(0);
    Vector3D a1 = edgeA.getVertex(1);
    Vector3D b0 = edgeB.getVertex(0);
    Vector3D b1 = edgeB.getVertex(1);

    // Calculate the direction vectors of the edges
    Vector3D dirA = a1 - a0;
    Vector3D dirB = b1 - b0;

    // Calculate the vector between the two edge starting points
    Vector3D offset = b0 - a0;

    // Calculate the determinant and squared lengths of the edges
    real det = dirA.vectorProduct(dirB).magnitude();
    real lenA = dirA.magnitudeSquared();
    real lenB = dirB.magnitudeSquared();

    // Calculate parameters for edge intersection
    real s = dirA.vectorProduct(offset).magnitude() / det;
    real t = dirB.vectorProduct(offset).magnitude() / det;

    // Check if edges intersect within their segments
    if (s >= 0 && s <= 1 && t >= 0 && t <= 1 && det > std::numeric_limits<real>::epsilon()) {
        // Calculate penetration depth (negative if edges overlap)
        real penetration = std::min(s, t);

        // If this penetration is shallower, we add a contact
        if (penetration < minPenetration) {
            minPenetration = penetration;

            // Calculate contact normal (perpendicular to the edges)
            contact.contactNormal = dirA.vectorProduct(dirB);
            contact.contactNormal.normalize();

            // Calculate contact point on edgeA
            contact.contactPoint = a0 + dirA * s;

            contact.body[0] = p1.body;
            contact.body[1] = p2.body;
        }
    }

    // If there was a collision, add the contact to the data vector
    if (minPenetration < std::numeric_limits<real>::max()) {
        contact.penetration = minPenetration;
        data.push_back(contact);
        return 1; // Indicate a collision was detected
    }

    return 0; // No collision detected
}

int pe::returnContacts(
    const Polyhedron& p1,
    const Polyhedron& p2,
    std::vector<Contact>& contactsToBeResolved
) {

    std::vector<Contact> contacts;
    for (const Vector3D& vertex : p1.globalVertices) {
        pointAndConvexPolyhedron(p2, vertex, p1, contacts);
    }
    for (const Vector3D& vertex : p2.globalVertices) {
        pointAndConvexPolyhedron(p1, vertex, p2, contacts);
    }

    /*
        Is it best not to have both point to face and edge to edge
        contacts in the same frame, as it can lead to unexpected
        behavior, so we only check for edge to edge contact when
        there aren't any point to face contacts.
    */
    if (contacts.empty()) {
        for (Edge* edge1 : p1.edges) {
            for (Edge* edge2 : p2.edges) {
                edgeToEdge(*edge1, *edge2, p1, p2, contacts);
            }
        }
    }

    /*
        Although we have this here just in case, we never expect the
        contact array to be empty, as we will first use SAT to make sure
        that the bodies p1 and p2 are colliding.
       */
    if (contacts.empty()) {
        // Handle the case where no contacts were found
        return 0;
    }

    for (const Contact& contact : contacts) {
        contactsToBeResolved.push_back(contact);
    }

    return contacts.size();
}