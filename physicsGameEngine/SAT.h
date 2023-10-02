/*
	Header file for utility functions that allow us to implement the
	Separating Axis Theorem (SAT) for collision detection between convex
	polyhedra with smooth uncurved surfaces of any king.
	The only needed data is the edge and face data, were the coordinates
	are always ordered in the same way (like clockwise) so that their normal,
	which is also needed, always faces outward, not inward.
*/

#ifndef SAT_H
#define SAT_H

#include "rigidBody.h"
#include "SFML/Graphics.hpp"
#include <vector>
#include "contact.h"

/*
	Note that both primitive and sat depend on each other, and we
	can only have one of them in a header file, so we make the SAT
	file depend on the primitive header, and make the primtive source file
	of the primitive depend on SAT to immeditaly implement collision
	detection within the primitive class.
*/
#include "primitive.h"

namespace pe {

	/* 
		Function that is used to check whether a line(defined by Pand D)
		separates the vertices of a polygon or not.
	*/
	int whichSide(const Primitive& C, const pe::Vector3D& P,
		const pe::Vector3D& D) {
		/*
			The vertices are projected to the form P + t * D. The return 
			value is +1 if all t>0, = 1 if all t<0, but 0 otherwise, in 
			which case the line splits the polygon projection.
		*/
		int positive = 0, negative = 0;
		for (int i = 0; i < C.globalVertices.size(); ++i) {
			// Project a vertex onto the line.
			pe::real t = D.scalarProduct(C.globalVertices[i] - P);
			if (t > 0) {
				++positive;
			}
			else if (t < 0) {
				++negative;
			}
			if (positive && negative) {
				/*
					The polygon has vertices on both sides of the line, so the
					line is not a separating axis. Time is saved by not having
					to project the remaining vertices.
				*/
				return 0;
			}
		}
		// Either positive > 0 or negative > 0 but not both are positive.
		return (positive > 0 ? +1 : -1);
	}

	// Function that returns true if the two primitives intersect
	bool testIntersection(const Primitive& C0, const Primitive& C1) {
		/*
			Test faces of C0 for separation. Because of the counter clockwise
			ordering, the  projection interval for C0 is [T,0] whereT<0.
			Determine whether C1 is on the  positive side of the line.
		*/
		for (int i = 0; i < C0.faces.size(); ++i) {
			pe::Vector3D P = *C0.faces[i].vertices[0];
			pe::Vector3D N = C0.faces[i].normal(); // outward pointing
			if (whichSide(C1, P, N) > 0) {
				// C1 is entirely on the positive side of the line P + t * N.
				return false;
			}
		}
		/*
			Test faces of C1 for separation. Because of the counterclockwise
			ordering, the  projection interval for C1 is [T,0] whereT<0.
			Determine whether C0 is on the  positive side of the line.
		*/
		for (int i = 0; i < C1.faces.size(); ++i) {
			pe::Vector3D P = *C1.faces[i].vertices[0];
			pe::Vector3D N = C1.faces[i].normal(); // outward pointing
			if (whichSide(C0, P, N) > 0) {
				// C1 is entirely on the positive side of the line P + t * N.
				return false;
			}
		}

		/*
			Test cross products of pairs of edge directions, one edge 
			direction from each polyhedron.
		*/
		for (int i0 = 0; i0 < C0.globalVertices.size(); ++i0) {
			pe::Vector3D D0 = *C0.edges[i0].vertices[1] - 
				*C0.edges[i0].vertices[0];
			pe::Vector3D P = *C0.edges[i0].vertices[0];
			for (int i1 = 0; i1 < C1.globalVertices.size(); ++i1) {
				pe::Vector3D D1 = *C1.edges[i1].vertices[1] - 
					*C1.edges[i1].vertices[0];
				pe::Vector3D N = D0.vectorProduct(D1);
				if (N != pe::Vector3D(0, 0, 0)) {
					int side0 = whichSide(C0, P, N);
					if (side0 == 0) {
						continue;
					}
					int side1 = whichSide(C1, P, N);
					if (side1 == 0) {
						continue;
					}
					if (side0 * side1 < 0) {
						/*
							The projections of C0 and C1 onto the line 
							P + t * N are on opposite sides of the projection
							of P.
						*/
						return false;
					}
				}
			}
		}
		return true;
	}
	
	/*
		Generates contacts between two primitives, be they face to face,
		point to face, face to edge, or edge to edge. Must be called
		after having made sure the two primitives are colliding,
		since this function only works assuming they are. It will wrongly
		generate contacts otherwise.

		NOTE: Untested.
	*/
	bool generateContacts(const Primitive& C0, const Primitive& C1,
		std::vector<Contact>& contacts) {

		// Test faces of C0 for separation
		for (int i = 0; i < C0.faces.size(); ++i) {
			Vector3D P = *C0.faces[i].vertices[0];
			Vector3D N = C0.faces[i].normal(); // outward pointing
			if (whichSide(C1, P, N) > 0) {
				// C1 is entirely on the positive side of the face.
				continue;
			}

			/*
				Calculate the contact point on the face of C0
				For simplicity, we'll assume the contact point is the 
				centroid of the face.
			*/
			Vector3D contactPointOnC0 = C0.faces[i].centroid();

			// Calculate the contact normal (opposite of the face normal)
			Vector3D contactNormal = N * -1;

			// Calculate penetration depth
			real penetration = (contactPointOnC0 - P).scalarProduct(N);

			// Create a contact
			Contact contact;
			contact.body[0] = C0.body;
			contact.body[1] = C1.body;
			contact.friction = 1;
			contact.restitution = 0.1;
			contact.contactPoint = contactPointOnC0;
			contact.contactNormal = contactNormal;
			contact.penetration = penetration;

			// Add the contact to the list
			contacts.push_back(contact);
		}

		// Test faces of C1 for separation (similar to the above loop)

		// Test cross products of pairs of edge directions
		for (int i0 = 0; i0 < C0.globalVertices.size(); ++i0) {
			Vector3D D0 = *C0.edges[i0].vertices[1] 
				- *C0.edges[i0].vertices[0];
			Vector3D P = *C0.edges[i0].vertices[0];
			for (int i1 = 0; i1 < C1.globalVertices.size(); ++i1) {
				Vector3D D1 = *C1.edges[i1].vertices[1] 
					- *C1.edges[i1].vertices[0];
				Vector3D N = D0.vectorProduct(D1);
				if (N != Vector3D(0, 0, 0)) {
					int side0 = whichSide(C0, P, N);
					if (side0 == 0) {
						continue;
					}
					int side1 = whichSide(C1, P, N);
					if (side1 == 0) {
						continue;
					}
					if (side0 * side1 < 0) {
						// Calculate contact point on edges
						Vector3D pointOnEdge0 = P + D1 
							* (D0.scalarProduct(D1) / D1.magnitudeSquared());
						Vector3D pointOnEdge1 = P + D0 
							* (D0.scalarProduct(D1) / D0.magnitudeSquared());

						/*
							Calculates penetration depth(distance between 
							the two closest points.
						*/ 
						Vector3D delta = pointOnEdge0 - pointOnEdge1;
						real penetration = delta.magnitude();

						/* 
							Normalizes the delta vector to get the contact
							normal.
						*/
						Vector3D contactNormal = delta;
						contactNormal.normalize();

						// Create a contact
						Contact contact;
						contact.body[0] = C0.body;
						contact.body[1] = C1.body;
						contact.friction = 1;
						contact.restitution = 0.1;
						// Midpoint between the two closest points
						contact.contactPoint = (pointOnEdge0 + pointOnEdge1) 
							* (real)0.5; 
						contact.contactNormal = contactNormal;
						contact.penetration = penetration;

						// Add the contact to the list
						contacts.push_back(contact);
					}
				}
			}
		}

		// Return true if there are contacts, false otherwise
		return !contacts.empty();
	}

	void resolveContact(const Contact& contact) {
		// Calculate relative velocity
		Vector3D relativeVelocity = (contact.body[1]->linearVelocity +
			contact.body[1]->angularVelocity.vectorProduct(
				contact.contactPoint - contact.body[1]->position)) -
			(contact.body[0]->linearVelocity +
				contact.body[0]->angularVelocity.vectorProduct(
					contact.contactPoint - contact.body[0]->position));

		// Calculate relative velocity along the contact normal
		double relativeVelocityAlongNormal 
			= relativeVelocity.scalarProduct(contact.contactNormal);

		// If relative velocity is separating, no action is needed
		if (relativeVelocityAlongNormal > 0) {
			return;
		}

		// Calculate impulse
		double impulse = -(1 + contact.restitution) 
			* relativeVelocityAlongNormal;
		impulse /= contact.body[0]->inverseMass 
			+ contact.body[1]->inverseMass;

		impulse *= 0.05;

		// Apply impulses to the bodies
		Vector3D impulseForce = contact.contactNormal * impulse;
		contact.body[0]->addForce(impulseForce, contact.contactPoint);
		contact.body[1]->addForce(impulseForce * -1, contact.contactPoint);

		// Calculate and apply friction impulses (if needed)
		// Implement friction calculations here

		/* 
			If objects are penetrating, move them apart along the contact 
			normal.
		*/
		if (contact.penetration > 0) {
			const double movePerMass = contact.penetration /
				(contact.body[0]->inverseMass 
					+ contact.body[1]->inverseMass);

			Vector3D move = contact.contactNormal * movePerMass;

			// Move objects apart
			contact.body[0]->position = (contact.body[0]->position +
				move * contact.body[0]->inverseMass);
			contact.body[1]->position = (contact.body[1]->position -
				move * contact.body[1]->inverseMass);
		}
	}
}

#endif