/*
	Interface for shapes that are almost cuboidal, meaning they benefit from
	having a box bounding volume.
*/

#ifndef CUBOIDAL_H
#define CUBOIDAL_H

#include "orientedBoundingBox.h"

namespace pe {

	class Cuboidal {
	public:

		OrientedBoundingBox boundingBox;

		Cuboidal(Polyhedron* polyhedron) : boundingBox(polyhedron) {}

	};
}

#endif