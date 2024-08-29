/*
	Header file for class representing an axis-aligned bounding box.
*/

#ifndef AXIS_ALIGNED_BOUNDING_BOX_H
#define AXIS_ALIGNED_BOUNDING_BOX_H

#include "boundingBox.h"

namespace pe {

	class AxisAlignedBoundingBox: public BoundingBox {

	private:

		/*
			Function that creates an AABB given a vector of points.
			The offset that is output is the centroid of the AABB
			relative to the centroid of the points (be it the polyhedron
			centroid or the origin of the world coordinate space in case
			the points are given in world coordinates).
		*/
		static void calculateAABB(
			const std::vector<Vector3D>& vertices,
			Vector3D& offset,
			Vector3D& halfsize
		) {

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
			offset = (maxCoord + minCoord) * 0.5;
		}

	public:

		AxisAlignedBoundingBox(
			Polyhedron* polyhedron
		) : BoundingBox(polyhedron) {
			update(polyhedron);
		}


		void update(Polyhedron* polyhedron) override {
			/*
				First we transform all the points we have ito world coordinates.
				Because the AABB must remain aligned to the world coordinate
				system, we can't calculate it once and transform it along with
				the polyhedron it encompasses.
				Instead, we are forced to calculate the AABB each frame, using
				the world coordinates of the polyhedron vertices. This means the
				offset we get each time is an offset from the world origin,
				meaning it is itself the centroid position of the AABB in world
				coordinates. The orientation of the AABB is always the identity
				as it is axis-aligned.
			*/
			std::vector<Vector3D> transformed(polyhedron->localVertices.size());
			for (int i = 0; i < transformed.size(); i++) {
				transformed[i] = polyhedron->getTransformMatrix().transform(
					polyhedron->localVertices[i]
				);
			}
			calculateAABB(transformed, baseOffset, halfsize);

			transformMatrix = Matrix3x4(Matrix3x3::IDENTITY, baseOffset);
		}

	};
}

#endif