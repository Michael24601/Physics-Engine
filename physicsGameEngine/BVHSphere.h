/*
	Header file for a bounding volume hierarchy sphere.
	The bounding volume type specifies the simple shape
	used in checking for a potential collision, like a sphere shape
	or prism shape. The bodies that are potentially colliding are
	inside these bounding spaces, are can only be colliding if the
	spaces themselves interpenetrate. The bounding volumes thus
	sometimes give false positives, where the spaces collide but not
	the objects, as the space won't always be a perfect fit. However,
	the simplicty of the space also makes the collision easier and
	faster to calculate, removing some of the burden from the slower
	sophisticated collision detection system. All in all, the coarse
	collision detection system's bounding volume spaces need to be
	accurate enough to eliminate most false positives, but not so
	accurate as to slow down the coarse collision detection, eliminating
	its original purpose as a fast but innacurate detection system.
*/

#ifndef BVH_SPHERE_H
#define BVH_SPHERE_H

#include "vector3D.h"

namespace pe {

	class BVHSphere {

	public:

		// centre coordinates and radius of the bounding volume's sphere
		Vector3D centre;
		real radius;

	public:

		// Constructs a bounding volume sphere with a centre and radius
		BVHSphere(const Vector3D& centre, real radius);

		/*
			Creates a bounding sphere that just barily contains two other
			bounding spheres. Used in creating a parent of two nodes in a
			Bounding Volume Hierarchy tree. Note that this isn't the
			smallest sphere containing its rigid body descendants, but
			rather, the smallest sphere encompassing its descendants'
			bounding spheres, which wastes more space but is better
			performance-wise.
		*/
		BVHSphere(
			const BVHSphere& sphere1,
			const BVHSphere& sphere2
		);

		/*
			Returns the volume of the bounding volume sphere.
		*/
		real getSize() const {
			return ((real)1.333333) * PI * radius * radius * radius;
		}

		// Returns true if the calling object overlaps with 
		bool overlaps(const BVHSphere* sphere) const;

		/*
			Returns how much the bounding volume sphere would have to grow
			in order to encompass a new bounding sphere given as a parameter.
			Used in order to determine where to insert a new body in the
			BVH tree. Note that the new growth is given in terms of area, not
			volume, as that is what the insertion algorithm needs to check.
		*/
		real getNewGrowth(const BVHSphere& newSphere) const;
	};
}

#endif