/*
	File for breakable interface.
*/

#ifndef BREAKABLE_H
#define BREAKABLE_H

#include "polyhedron.h"

namespace pe {

	class Breakable {

		virtual std::vector<Polyhedron*> breakObject() = 0;
	};
}



#endif