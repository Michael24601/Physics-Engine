/*
	Interface for shapes that are almost spherical, meaning they benefit from
	having a sphere bounding volume.
*/

#ifndef SPHERICAL_H
#define SPHERICAL_H

#include "boundingSphere.h"

namespace pe {

	class Spherical: public Polyhedron {
	public:

		BoundingSphere boundingSphere;

		Spherical(
			real mass,
			const Vector3D& position,
			const Matrix3x3& inertiaTensor,
			const std::vector<Vector3D>& localVertices,
			RigidBody* body
		) : Polyhedron(mass, position, inertiaTensor, localVertices, body),
			boundingSphere(this) {}



		void update() {
			boundingSphere.update(this);
		}

	};
}

#endif