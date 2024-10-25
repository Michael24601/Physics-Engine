/*
	Header file for class representing a bounding box.
	This class extends the functionality of the bounding volume class,
	specifying the shape of the volume (being a box, or rectangulat prism).
	However, it is still virtual, as the method of creating the bounding
	volume is not specified here, as there are many such methods (oriented
	bounding and axis-aligned bounding box).
*/

#ifndef BOUNDING_BOX_H
#define BOUNDING_BOX_H

#include "boundingVolume.h"

namespace pe{

	class BoundingBox : public BoundingVolume {

	protected:

		// The size of the box
		Vector3D halfsize;

	public:

		BoundingBox() :
			BoundingVolume(Vector3D::ZERO, Matrix3x3::IDENTITY),
			halfsize{ Vector3D::ZERO } {}


		BoundingBox(
			const Vector3D& position, 
			const Matrix3x3& orientation,
			const Vector3D& halfsize
		) : BoundingVolume(position, orientation), halfsize{halfsize} {}


		Vector3D getHalfsize() const {
			return halfsize;
		}


		/*
			The smallest sphere encompassing a box (centered at the
			same place) is the one with the halfsize as its radius.
		*/
		real getBVHSphereRadius() const override {
			return halfsize.magnitude();
		}

	};
}

#endif