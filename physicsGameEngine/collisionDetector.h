/*
	In the fine collision detection system, we have to consider all the
	different pairs of geometric shapes that may be in contact, and the
	different types of contacts they may be in depending on their shape.
	This is the header file of the class containing the static 
*/

#ifndef COLLISION_DETECTOR_H
#define COLLISION_DETECTOR_H

#include "sphere.h"
#include "plane.h"
#include "rectangularPrism.h"
#include "contactData.h"

namespace pe {

	class CollisionDetector {

	public:

		/*
			Checks if there is a contact between the two given spheres,
			and fills the ContactData structure with as many contacts as
			are generated without exceeding its limit. Returns the number
			of generated contacts. Because both are spheres, the only type
			of contact is face to face contact.
		*/
		static unsigned int sphereAndSphere(const Sphere& one,
			const Sphere& two, ContactData* data);

		/*
			A plane and a sphere have a simple collision, which is always
			face to face. 
		*/
		static unsigned int sphereAndPlane(const Sphere& sphere,
			const Plane& plane, ContactData* data);

		/*
			A plane and a box collide when at least of of its eight vertices
			passes through the plane. Although we can have face to face,
			edge to face, and poitn to face contacts, we can simply replace
			those by poit to face contacts on the edges that collide with
			the plane, convincingly enough simulating the other types of
			contacts.
		*/
		static unsigned int boxAndPlane(const RectangularPrism& box,
			const Plane& plane, ContactData* data);
	};
}

#endif