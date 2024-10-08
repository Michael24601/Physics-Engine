/*
	Interface for shapes that are almost cuboidal, meaning they benefit from
	having a box bounding volume.
*/

#ifndef CUBOIDAL_H
#define CUBOIDAL_H

#include "orientedBoundingBox.h"

namespace pe {

	class Cuboidal: public Polyhedron {
	public:

		OrientedBoundingBox boundingBox;

		Cuboidal(
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