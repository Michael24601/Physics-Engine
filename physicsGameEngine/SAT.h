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
#include "polyhedron.h"

namespace pe {

	/* 
		Function that is used to check whether a line(defined by Pand D)
		separates the vertices of a polygon or not.
	*/
	int whichSide(
		const Polyhedron& C,
		const pe::Vector3D& P,
		const pe::Vector3D& D
	);


	// Function that returns true if the two primitives intersect
	bool testIntersection(const Polyhedron& C0, const Polyhedron& C1);
}

#endif