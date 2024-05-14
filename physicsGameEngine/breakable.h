/*
	File for breakable interface.
*/

#ifndef BREAKABLE_H
#define BREAKABLE_H

#include "polyhedron.h"
#include "contact.h"

namespace pe {

	class Breakable {

		virtual void breakObject(
			std::vector<Polyhedron*>& polyhedra, 
			Vector3D dimensionPoint,
			Vector3D breakingPoint
		) = 0;
	};
}



#endif