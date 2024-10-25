/*
	Header file for class representing an axis-aligned bounding box.

	The box can't be transformed using a transform matrix because it
	is meant to remain axis aligned, and must be recalculated if used
	on a moving, rotating rigid body.
*/

#ifndef AXIS_ALIGNED_BOUNDING_BOX_H
#define AXIS_ALIGNED_BOUNDING_BOX_H

#include "boundingBox.h"

namespace pe {

	class AxisAlignedBoundingBox: public BoundingBox {

	public:


		AxisAlignedBoundingBox() :
			BoundingBox(Vector3D::ZERO, Matrix3x3::IDENTITY, Vector3D::ZERO) {}


		AxisAlignedBoundingBox(
			const Vector3D& position,
			const Matrix3x3& orientation,
			const Vector3D& halfsize
		) : BoundingBox(position, orientation, halfsize) {}


		/*
			Function that creates an AABB given a vector of points.
			The offset that is output is the centroid of the AABB
			relative to the centroid of the points (be it the polyhedron
			centroid or the origin of the world coordinate space in case
			the points are given in world coordinates).
		*/
		void fit(const std::vector<Vector3D>& vertices) {

			if (vertices.empty()) {
				return;
			}

			// Initialize min and max coordinates with the first vertex
			Vector3D minCoord = vertices[0];
			Vector3D maxCoord = vertices[0];

			// Iterate through all vertices to find the minimum and maximum coordinates
			for (const Vector3D& vertex : vertices) {
				// The vertices are assumed to be in global coordinates
				minCoord.x = std::min(minCoord.x, vertex.x);
				minCoord.y = std::min(minCoord.y, vertex.y);
				minCoord.z = std::min(minCoord.z, vertex.z);

				maxCoord.x = std::max(maxCoord.x, vertex.x);
				maxCoord.y = std::max(maxCoord.y, vertex.y);
				maxCoord.z = std::max(maxCoord.z, vertex.z);
			}

			/*
				Calculating halfsize by taking half of the distance between
				min and max coordinates.
			*/
			halfsize = (maxCoord - minCoord) * 0.5;

			/*
				The offset will be equal to offset - centre of gravity = offset - 0
				as the centre of gravity is 0 in local coordinates.
			*/
			position = (maxCoord + minCoord) * 0.5;

			// The AABB is always orthogonal
			orientation = Matrix3x3::IDENTITY;
		}
	};
}

#endif