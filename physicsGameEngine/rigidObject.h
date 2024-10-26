/*
	A rigid object class that includes a body (for physics),
	a mesh, and a boudning volume/
	The class is flexible in the sense that it accepts the mesh,
	bounding volume, and body from outside. So the bounding volume
	doesn't need to fit the mesh for instance, it is up to the
	caller to send whatever bounding volume they may need,
	instead of fitting the mesh inside the class.
	Likewise, if a mesh is very complex, we can create one
	and share it among all objects that have this mesh shape,
	that way only one copy of it has to exist.
	Likewise a bounding volume can be shared between different
	objects that have the same mesh, as fitting can be expensive.
*/

#ifndef RIGID_OBJECT_H
#define RIGID_OBJECT_H

#include "rigidBody.h"
#include "rigidBodyForceGenerator.h"
#include "boundingVolume.h"

namespace pe {

	template<typename BoundingVolumeClass>
	class RigidObject {

		// Ensuring that BoundingVolumeClass inherits from BoundingVolume
		static_assert(
			std::is_base_of<BoundingVolume, BoundingVolumeClass>::value,
			"BoundingVolumeClass must inherit from BoundingVolume"
			);

	public:


		// The body, which handles the physics
		RigidBody body;


		// The mesh
		const Mesh* mesh;


		// The preferred bounding volume shape for collisions
		const BoundingVolumeClass* boundingVolume;


		RigidObject(
			Mesh* mesh,
			BoundingVolumeClass* boundingVolume,
			const Vector3D& position = Vector3D::ZERO,
			const Quaternion& orientation = Quaternion::IDENTITY,
			real mass = 0,
			const Matrix3x3& inertiaTensor = Matrix3x3::IDENTITY
		) : mesh{mesh},
			boundingVolume{boundingVolume},
			body(mass, inertiaTensor, position, orientation) {

			assert(mesh != nullptr && "Mesh cannot be null");
			assert(boundingVolume != nullptr && "Bounding volume cannot be null");

			// Initializes data
			update();
		}


		/*
			Function that updates values that need to be changed when
			the body moves. Called after forces are applied to the body
			and the body has been integrated.
		*/
		void update() {
			// Calculates the derived data, like the transform matrix
			body.calculateDerivedData();
		}
	};
}

#endif