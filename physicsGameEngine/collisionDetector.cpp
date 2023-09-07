
#include "collisionDetector.h"

#include <iostream>

using namespace pe;

unsigned int CollisionDetector::sphereAndSphere(const Sphere& one,
	const Sphere& two, ContactData* data) {

	// Make sure we haven't already exceeded the limit of contacts
	if (data->contactsLeft <= 0) {
		return 0;
	}

	/*
		Get the sphere positions from the third column of the transform
		matrices.
	*/
	Vector3D onePosition = one.getAxis(3);
	Vector3D twoPosition = two.getAxis(3);

	// Vector between the two spheres and its length
	Vector3D vector = onePosition - twoPosition;
	real size = vector.magnitude();

	// If they are too far apart to collide
	if (size <= 0.0f || size >= one.radius + two.radius) {
		return 0;
	}

	/*
		Otherwise, there will be a collision, which needs to be resolved
		through exactly one contact.
	*/
	Vector3D normal = vector * (((real)1.0) / size);

	// Array of contacts to which our contact will be appended
	Contact* contact = data->contacts;

	contact->contactNormal = normal;
	contact->contactPoint = onePosition + vector * (real)0.5;
	contact->penetration = (one.radius + two.radius - size);

	// Write the appropriate data.
	contact->body[0] = one.body;
	contact->body[1] = two.body;
	contact->restitution = data->restitution;
	contact->friction = data->friction;

	return 1;
}

unsigned int CollisionDetector::sphereAndPlane(const Sphere& sphere,
	const Plane& plane, ContactData* data) {

	// Make sure we haven't already exceeded the limit of contacts
	if (data->contactsLeft <= 0) {
		return 0;
	}

	/*
		Get the sphere position from the third column of the transform
		matrices.
	*/
	Vector3D position = sphere.getAxis(3);

	// Distance between sphere and plane
	real distance = plane.normal.scalarProduct(position) -
		sphere.radius - plane.offset;

	// If they are not touching, no contacts are filled
	if (distance >= 0) {
		return 0;
	}

	// Creates the contact which has a normal in the plane direction
	Contact* contact = data->contacts;
	contact->contactNormal = plane.normal;
	contact->penetration = -distance;
	contact->contactPoint = position - plane.normal *
		(distance + sphere.radius);

	// Then writes the appropriate data
	contact->body[0] = sphere.body;
	contact->body[1] = nullptr;
	contact->restitution = data->restitution;
	contact->friction = data->friction;

	return 1;
}

unsigned int CollisionDetector::boxAndPlane(const RectangularPrism& box,
	const Plane& plane, ContactData* data) {

	int contactCount{};

	/*
		The collisions are generated as point to face collisions for each
		of the 8 vertices of a rectangular prism, so we have to loop over
		all eight. We use the absolute position of the vertices, which 
		are recalculated each frame using the relative position and the
		transform matrix. We also here add the offset.
	*/
	for (int i = 0; i < 8; i++) {

		
		Vector3D vertexPosition = box.body->transformMatrix
			* box.vertices[i];

		// Distance from the plane of one of the vertices
		real vertexDistance = (vertexPosition.scalarProduct(plane.normal)
			+ plane.offset) / (plane.normal.magnitude()) ;

		// Checks if the vertex is colliding with the plane
		if (vertexDistance <= data->tolerance) {
			/*
				Creates a contact with the point at the halfway distance
				between the planeand the vertex.
			*/
			Contact* contact = data->contacts;
			contact->contactPoint = plane.normal;
			contact->contactPoint *= (vertexDistance - plane.offset);
			contact->contactPoint = vertexPosition;
			contact->contactNormal = plane.normal;
			contact->penetration = plane.offset - vertexDistance;

			contactCount++;
		}
	}
	return contactCount;
}