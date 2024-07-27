/*
	Header file for virtual class representing the bounding volume of a
	3D shape.
*/

#ifndef BOUNDING_VOLUME_H
#define BOUNDING_VOLUME_H

#include "polyhedron.h"

namespace pe {

	class BoundingVolume {

	protected:

		/*
			The polyhedron the bounding volume encompasses.
		*/
		Polyhedron* polyhedron;


		/*
			Similar to the polyhedron, the bounding volume will have its own
			transformation parameters that incorporates its orientation and
			position in relation to the local coordinates of the polyhedron
			(relative to the centroid of the polyhedron its axes).
			This is because the centroid of the bounding volume may not be
			at the centroid of the polyhedron (collection of vertices) it
			encompasses. Likewise, the bounding volume might not initially
			be aligned with the object's axes. This is because certain
			bounding volumes may end up off centre and rotated in a bid to
			be the best fit possible.
			These parameters, the initial offset and rotation, tell us where
			the bounding box is in relation to the centroid of the vertices
			that were sent.
		*/
		Matrix3x3 baseOrientation;

		// The original offset (relative to the object's centroid)
		Vector3D baseOffset;

		/*
			The offset and orientation are relative to the polyhedron
			the volumes are bound to. In order to know the position and
			orientation of the objects in world coordinates, we may 
			however need to combine these with the transform matrix of
			the polyhedron itself. This is the case with bounding volumes
			such as oriented bounding boxes, but is not the case with
			others, such as axis-aligned bounding boxes, which need to be
			recalculated each frame. 
			Either way, the true orientation and position are stored in
			this 3 by 4 matrix, which is updated each frame.
		*/
		Matrix3x4 transformMatrix;

	public:

		// The vertices are expected to be in relative coordinates
		BoundingVolume(Polyhedron* polyhedron) :
			baseOrientation{ Matrix3x3::IDENTITY },
			baseOffset{ Vector3D::ZERO },
			transformMatrix{ Matrix3x4::IDENTITY } {}

		/*
			Updates the bounding volume and its transform matrix
			(possibly using the polyhedron's transform matrix).
		*/
		virtual void update() = 0;


		Matrix3x4 getTransformMatrix() const {
			return transformMatrix;
		}


		Vector3D getPosition() const {
			return transformMatrix.getTranslation();
		}

	};

}

#endif