/*
	File for breakable interface.
*/

#ifndef BREAKABLE_H
#define BREAKABLE_H

#include "polyhedron.h"

namespace pe {

	class Breakable {

		virtual void breakObject(
			std::vector<Polyhedron*>& polyhedra, 
			const Vector3D& contactNormal, 
			real deltaT
		) = 0;
	};
}



#endif