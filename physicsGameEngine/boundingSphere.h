
#ifndef BOUNDING_SPHERE_H
#define BOUNDING_SPHERE_H

#include "vector3D.h"

namespace pe{

	class BoundingSphere {

	private:

		// centre coordinates and radius of the bounding volume's sphere
		Vector3D centre;
		real radius;

	public:

		// Constructs a bounding volume sphere with a centre and radius
        BoundingSphere(const Vector3D& centre, real radius);

		/*
			Creates a bounding sphere that just barily contains two other
			bounding spheres. Used in creating a parent of two nodes in a
			Bounding Volume Hierarchy tree.
		*/
		BoundingSphere(const BoundingSphere& sphere1,
			const BoundingSphere& sphere2);

		// Returns true if the calling object overlaps with 
		bool overlaps(const BoundingSphere* sphere) const;
	};
}

#endif