/*
	Header file for virtual class representing the bounding volume of a
	3D shape.
*/

#ifndef BOUNDING_VOLUME_H
#define BOUNDING_VOLUME_H

#include "mesh.h"
#include <vector>

namespace pe {

	class BoundingVolume {

	protected:

		/*
			The orientation of the bounding volume.
			Note that if the object being fit is in local coordinates,
			we will get the bounding volume orientation in local coordinates,
			which can then be transformed with the object's transform matrix.
		*/
		Matrix3x3 orientation;

		/*
			The position of the bounding volume.
			Note that if the object being fit is in local coordinates,
			we will get the bounding volume position in local coordinates,
			which can then be transformed with the object's transform matrix.
		*/
		Vector3D position;

	public:

		BoundingVolume() :
			position{ Vector3D::ZERO }, 
			orientation{ Matrix3x3::IDENTITY } {}


		BoundingVolume(const Vector3D& position, const Matrix3x3& orientation) :
			position{ position }, orientation{ orientation } {}


		/*
			Fits the volume to a mesh.
			Note that if we already had a position and orientation and size
			set, this function overrides them.

			Note that depending on the bounding volume and shape we have,
			it may be best to only call fit at the start, when the object
			is in local coordinates, and keep the bounding volume in the
			relative local basis. We can then use the transform matrix of
			the object and combine it with the position and orientation of
			the bounding volume to get the bounding volume location. This
			is best done since the fit method is usually very slow, and can't
			be called each frame.
			The only exception is shapes that can't be transformed, such as
			AABB, which must remain axis aligned.
		*/
		virtual void fit(const std::vector<Vector3D>& vertices) = 0;


		/*
			Returns the radius of the smallest sphere centered at
			the same cooardinate as the volume, containing the volume.

			Because the bounding volume hierarchy uses the sphere as a 
			simple shape through which the coarse collision detection
			phase can be done, all bounding volumes must be able to
			return the smallest bounding sphere containing them, so that
			the coarse collision detection phase can be done regardless
			of what bounding volume an object has.
			This won't necessarily be the smallest sphere containing the
			object, but it will be the smallest sphere containing the
			bounding volume.

			This is close enough to what we need and very fast to calculate.
		*/
		virtual real getBVHSphereRadius() const = 0;


		/*
			Returns the position and orientation as a homogeneous matrix.
		*/
		Matrix3x4 getTransformMatrix() const {
			return Matrix3x4(orientation, position);
		}

	};
}

#endif