/*
	Interface for shapes that are almost cuboidal, meaning they benefit from
	having a box bounding volume. In this case the bounding box is axis
	aligned.
*/

#ifndef ORTHOGONAL_H
#define ORTHOGONAL_H

#include "axisAlignedBoundingBox.h"

namespace pe {

	class Orthogonal : public Polyhedron {
	public:

		AxisAlignedBoundingBox boundingBox;

		Orthogonal(
			real mass,
			const Vector3D& position,
			const Matrix3x3& inertiaTensor,
			const std::vector<Vector3D>& localVertices,
			RigidBody* body
		) : Polyhedron(mass, position, inertiaTensor, localVertices, body),
			boundingBox(this) {}


		void update() {
			boundingBox.update(this);
		}

	};
}

#endif