/*
	Interface for shapes that are almost spherical, meaning they benefit from
	having a sphere bounding volume.
*/

#ifndef SPHERICAL_H
#define SPHERICAL_H

#include "boundingSphere.h"

namespace pe {

	class Spherical {
	public:

		BoundingSphere boundingSphere;

		Spherical(Polyhedron* polyhedron) : boundingSphere(polyhedron) {}

	};
}

#endif