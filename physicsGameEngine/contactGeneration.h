
#include "primitive.h"
#include "contact.h"

namespace pe {

    // The second polyhedron is sent in order to use it to set the body
    // array in the contact.
    unsigned pointAndConvexPolyhedron(
        const Primitive& polyhedron,
        const Vector3D& point,
        const Primitive& secondPolyhedron,
        std::vector<Contact>& data) {

        // Transform the point into the local space of the polyhedron
        Vector3D localPoint 
            = polyhedron.body->transformMatrix.inverseTransform(point);

        // Initialize variables to track the closest face and minimum penetration depth
        Vector3D closestFaceNormal;
        real minDepth = REAL_MAX;  // Set to a large initial value

        // Iterate through each face of the convex polyhedron
        for (size_t i = 0; i < polyhedron.faces.size(); i++)
        {
            // Get the face normal and any point on the face in local space
            Vector3D faceNormal = polyhedron.faces[i].normal();
            Vector3D pointOnFace = *polyhedron.faces[i].vertices[0];

            // Compute the vector from the point to the point on the face
            Vector3D toPointOnFace = pointOnFace - localPoint;

            // Compute the penetration depth along the face normal
            real depth = faceNormal.scalarProduct(toPointOnFace);

            // Check if the point is inside or in front of the face
            if (depth > 0)
            {
                // The point is in front of the face, meaning it's on the outside of the polyhedron
                return 0;
            }

            // If the depth is smaller than the current minimum, update the minimum depth and closest face normal
            if (depth < minDepth)
            {
                minDepth = depth;
                closestFaceNormal = faceNormal;
            }
        }

        // Compile the contact
        Contact contact;
        // Transform the normal back to world space
        contact.contactNormal = polyhedron.body->transformMatrix.transform(closestFaceNormal);
        contact.contactPoint = point;
        contact.penetration = -minDepth; // Negative depth since the point is inside the polyhedron
        // We want the body[0] to be the one in whose direction the contact normal is,
        // and the other having the opposite contact normal.
        // So body[0] is the one to whom the point belongs, and body[1] is the primitive
        // sent to this function.
        contact.body[0] = secondPolyhedron.body;
        contact.body[1] = polyhedron.body;

        data.push_back(contact);

        return 1;
    }


    // We again send the primitives to be used in filling the contact array
    unsigned edgeToEdge(
        const Edge& edgeA,
        const Edge& edgeB,
        const Primitive& p1,
        const Primitive& p2,
        std::vector<Contact>& data){

        // Initialize variables to track the shallowest penetration and corresponding edge pair
        real minPenetration = REAL_MAX;
        Contact contact;

        // Iterate through each pair of edges (one from edgeA and one from edgeB)
        for (size_t i = 0; i < edgeA.vertices.size() - 1; ++i)
        {
            for (size_t j = 0; j < edgeB.vertices.size() - 1; ++j)
            {
                // Get the two vertices of the current edges
                Vector3D a0 = *edgeA.vertices[i];
                Vector3D a1 = *edgeA.vertices[i + 1];
                Vector3D b0 = *edgeB.vertices[j];
                Vector3D b1 = *edgeB.vertices[j + 1];

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

                // Check if edges intersect within their segments (0 <= s <= 1 and 0 <= t <= 1)
                if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
                {
                    // Calculate penetration depth (negative if edges overlap)
                    real penetration = (s < t) ? s : t;

                    // If this penetration is shallower, update contact information
                    if (penetration < minPenetration)
                    {
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

    bool returnMaxContact(const Primitive& p1, const Primitive& p2, 
        std::vector<Contact>& contactsToBeResolved) {

        std::vector<Contact> contacts;
        for (const Vector3D& vertex : p1.globalVertices) {
            pointAndConvexPolyhedron(p2, vertex, p1, contacts);
        }
        for (const Vector3D& vertex : p2.globalVertices) {
            pointAndConvexPolyhedron(p1, vertex, p2, contacts);
        }
        for (const Edge& edge1 : p1.edges) {
            for (Edge edge2 : p2.edges) {
                edgeToEdge(edge1, edge2, p1, p2, contacts);
            }
        }

        // Although we have this here just in case, we never expect the contact
        // array to be empty, as we will first use SAT to make sure
        // that the bodies p1 and p2 are colliding.
        if (contacts.empty()) {
            // Handle the case where no contacts were found
            return false;
        }

        // Return the contact with the most penetration (one needing
        // the most to be resolved)
        // Later may also add relative velocity vector and choose
        // the contact with the best penetartion and contact normal closest
        // to this vector's direction
        Contact maxContact = *max_element(contacts.begin(), contacts.end(),
            [](const Contact& c1, const Contact& c2) -> bool {
                return c1.penetration > c2.penetration;
            });

        // This part does that, checks the one with the normal closest
        // to the velocity direction.
        Contact prioritizedContact;
        Vector3D relativeVelocity = p1.body->linearVelocity - p2.body->linearVelocity;
        relativeVelocity.normalize();
        real highestDotProduct = -REAL_MAX;
        // Iterate through contacts and prioritize based on dot product
        for (const Contact& contact : contacts) {
            // Calculate the dot product
            real dotProduct = relativeVelocity.scalarProduct(contact.contactNormal);
            // Prioritize contacts based on dot product
            // Note that we care about alignment with the direction of the normal
            // not necessatily with the same sign, we use the aboslute value of the
            // dot product
            if (realAbs(dotProduct) > highestDotProduct) {
                highestDotProduct = realAbs(dotProduct);
                prioritizedContact = contact;
            }
        }
        contactsToBeResolved.push_back(prioritizedContact);

        return true;
    }
}