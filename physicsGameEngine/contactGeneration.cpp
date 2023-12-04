
#include "contactGeneration.h"

using namespace pe;


unsigned int pe::pointAndConvexPolyhedron(
    const Polyhedron& polyhedron,
    const Vector3D& point,
    const Polyhedron& secondPolyhedron,
    std::vector<Contact>& data) {

    // Transform the point into the local space of the polyhedron
    Vector3D localPoint
        = polyhedron.body->transformMatrix.inverseTransform(point);

    // Initialize variables to track the closest face
    Vector3D closestFaceNormal;
    // And track the minimum penetration depth
    real minDepth = REAL_MAX;

    // Iterate through each face of the convex polyhedron
    for (size_t i = 0; i < polyhedron.faces.size(); i++)
    {
        // Get the face normal and any point on the face in local space
        Vector3D faceNormal = polyhedron.faces[i].normal;
        Vector3D pointOnFace = polyhedron.faces[i].vertices[0];

        // Compute the vector from the point to the point on the face
        Vector3D toPointOnFace = pointOnFace - localPoint;

        // Compute the penetration depth along the face normal
        real depth = faceNormal.scalarProduct(toPointOnFace);

        // Check if the point is inside or in front of the face
        if (depth > 0)
        {
            /*
                The point is in front of the face, meaning it's on the
                outside of the polyhedron.
            */
            return 0;
        }

        /*
            If the depth is smaller than the current minimum, update the
            minimum depthand closest face normal.
        */
        if (depth < minDepth)
        {
            minDepth = depth;
            closestFaceNormal = faceNormal;
        }
    }

    Contact contact;
    // We transform the normal back to world space
    contact.contactNormal = polyhedron.body->transformMatrix.transform(
        closestFaceNormal);
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

    // We initialize variables to track the shallowest penetration
    real minPenetration = REAL_MAX;
    // And corresponding edge pair
    Contact contact;

    // Get the two vertices of the current edges
    Vector3D a0 = edgeA.vertices.first;
    Vector3D a1 = edgeA.vertices.second;
    Vector3D b0 = edgeB.vertices.first;
    Vector3D b1 = edgeB.vertices.second;

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
    if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
    {
        // Calculate penetration depth (negative if edges overlap)
        real penetration = (s < t) ? s : t;

        // If this penetration is shallower, we add a contact
        if (penetration < minPenetration)
        {
            minPenetration = penetration;

            // Calculate contact normal (perpendicular to the edges)
            contact.contactNormal = dirA.vectorProduct(dirB);
            contact.contactNormal.normalize();

            // Calculate contact point on edgeA
            contact.contactPoint = a0 + dirA * s;

            /*
                body[0] is the one in respect to whom the contact
                normal is pointing. The contact normal of body[1] is
                the opposite of that which is saved in the contact.
            */
            contact.body[0] = p1.body;
            contact.body[1] = p2.body;
        }
    }

    // If there was a collision, add the contact to the data vector
    if (minPenetration < REAL_MAX)
    {
        contact.penetration = minPenetration;
        data.push_back(contact);
        return 1; // Indicate a collision was detected
    }

    return 0; // No collision detected
}


/*
    In this function we test all vertices in both polyhedra against the
    other, as well as all combinations of edges. Trading accuracy for
    performance, we then resolve only one of them. There are several
    criteria to follow in choosing which contact to resolve for this
    collision. For instance, we can think of the one with the most
    penetration depth as being the most urgent.
    In our case, we choose the one closest to the velocity (travel)
    direction of the two objects.
*/
bool pe::returnMaxContact(const Polyhedron& p1, const Polyhedron& p2,
    std::vector<Contact>& contactsToBeResolved) {

    std::vector<Contact> contacts;
    for (const Vector3D& vertex : p1.globalVertices) {
        pointAndConvexPolyhedron(p2, vertex, p1, contacts);
    }
    for (const Vector3D& vertex : p2.globalVertices) {
        pointAndConvexPolyhedron(p1, vertex, p2, contacts);
    }
    for (const Edge& edge1 : p1.edges) {
        for (const Edge& edge2 : p2.edges) {
            edgeToEdge(edge1, edge2, p1, p2, contacts);
        }
    }

    /*
        Although we have this here just in case, we never expect the
        contact array to be empty, as we will first use SAT to make sure
        that the bodies p1 and p2 are colliding.
       */
    if (contacts.empty()) {
        // Handle the case where no contacts were found
        return false;
    }

    /*
        This part does that, checks the one with the normal closest
        to the velocity direction.
    */
    Contact prioritizedContact;
    Vector3D relativeVelocity = p1.body->linearVelocity
        - p2.body->linearVelocity;
    relativeVelocity.normalize();
    real highestDotProduct = -REAL_MAX;

    // We iterate through contacts and prioritize based on the dot product
    for (const Contact& contact : contacts) {

        real dotProduct = relativeVelocity.scalarProduct(
            contact.contactNormal);

        /*
            Note that we care about alignment with the direction of the
            normal not necessatily with the same sign, we use the
            aboslute value of the dot product.
        */
        if (realAbs(dotProduct) > highestDotProduct) {
            highestDotProduct = realAbs(dotProduct);
            prioritizedContact = contact;
        }
    }

    Contact maxContact = *max_element(contacts.begin(), contacts.end(),
        [](const Contact& c1, const Contact& c2)->bool {
            return c1.penetration > c2.penetration;
        });


    // contactsToBeResolved.push_back(prioritizedContact);
    contactsToBeResolved.push_back(maxContact);

    return true;
}