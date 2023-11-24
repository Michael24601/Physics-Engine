/*
	Header file for a plane class, which is a 2D object in 3D space
	commonly thoght of as immovable, so there is no rigid body associated
	with it.
	Note that this isn't a true plane, but a half-space, which is more
	useful in most gaming engines. The difference is that the half-space
	can only detect collisions on one side, and always resolves the contact
	by pushing the body towards its normal, never the opposite direction,
	even if the object came from there.
	We would need to handle the case where the object is closer to the other
	side then to this one and use an opposite normal in order to have a
	working plane, but that is hardly necessary. So a collision can only 
	happen coming against the normal.
*/

#ifndef PLANE_H
#define PLANE_H

#include "vector3D.h"

namespace pe {
	
	class Plane {

	public:

		/*
			The normal of the plane, which is enough to define it (it has
			infinite area on all sides). We only then need to know how
			far it is from the origin. The components of the vector are thus
			the first three coefficients of the function
			Ax + By + Cz + D = 0. The vector must be normal.
		*/
		Vector3D normal;

		/*
			Distance of the plane from the origin. This means that the
			plane has the equation: Ax + By + Cz + D = 0 where A, B, and C
			are the normal's coordinates, and D is the offset.
			So this offset is the distance of the plane from the origin in
			the normal's direction, with the side depending on the offset's
			sign.
			This is different from the offset matrix in the geometricShape
			interface that this class inherits from.
		*/
		real offset;

		// Argumented constructor
		Plane(const Vector3D& normal, real offset) :
			normal{ normal }, offset{offset} {}

		/*
			Draws nothing for now.
		*/
		virtual std::vector<sf::VertexArray> drawLines() {
			std::vector<sf::VertexArray> lines;
			return lines;
		}

	};
}

#endif
