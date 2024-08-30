/*
	Header file for class representing a bounding sphere.
	The shape is simple enough that it can be used in coarse collision
	detection, which means that it should fuffil the requirements of
	the Resizable concept.
*/

#ifndef BOUNDING_SPHERE_H
#define BOUNDING_SPHERE_H

#include "boundingVolume.h"

namespace pe {

	class BoundingSphere : public BoundingVolume {

	protected:

		// The radius of the sphere
		real radius;

	public:


		BoundingSphere(
			Polyhedron* polyhedron
		) : BoundingVolume(polyhedron) {

			/*
				Calculates the smallest bounding sphere that encompasses
				the given vertices.
				This is different from finding the point furthest from the
				center of the given polyhedron, as the smallest sphere might
				not be centered at the centroid.
			*/

			if (polyhedron->localVertices.empty()) {
				return;
			}

			// Initializes the center to the first point
			Vector3D center = polyhedron->localVertices[0];

			/*
				First we find the center of the bounding sphere which
				may differ from the centre of gravity if we wish the bounding
				sphere to be minimal.
			*/
			for (const Vector3D& point : polyhedron->localVertices) {
				center.x += point.x;
				center.y += point.y;
				center.z += point.z;
			}
			center.x /= polyhedron->localVertices.size();
			center.y /= polyhedron->localVertices.size();
			center.z /= polyhedron->localVertices.size();

			// We then calculates its radius
			radius = (real)0.0;
			for (const Vector3D& point : polyhedron->localVertices) {
				float distance = std::sqrt(
					(point.x - center.x) * (point.x - center.x) +
					(point.y - center.y) * (point.y - center.y) +
					(point.z - center.z) * (point.z - center.z)
				);
				if (distance > radius) {
					radius = distance;
				}
			}

			baseOffset = center;
		}


		real getRadius() const {
			return radius;
		}


		void update(Polyhedron* polyhedron) override {
			/*
				Because the sphere does not change radius regardless of
				how the polyhedron rotates or moves, we can update the
				sphere transform matrix by just combining its base translation
				with the polyhedron's transform matrix.
				The orientation always remains the identity as a sphere has none.
			*/
			transformMatrix.setTranslation(
				polyhedron->getTransformMatrix().transform(baseOffset)
			);
		}
	};
}

#endif