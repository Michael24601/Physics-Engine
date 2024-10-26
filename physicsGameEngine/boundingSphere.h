/*
	Header file for class representing a bounding ball.
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


		BoundingSphere() :
			BoundingVolume(Vector3D::ZERO, Matrix3x3::IDENTITY),
			radius{ 0 } {}


		BoundingSphere(
			real radius,
			const Vector3D& position = Vector3D::ZERO
		) : BoundingVolume(position, Matrix3x3::IDENTITY), radius{radius} {}


		TYPE getType() const override {
			return TYPE::SPHERE;
		}


		void fit(const std::vector<Vector3D>& vertices) {
			/*
				Calculates the smallest bounding sphere that encompasses
				the given vertices.
				This is different from finding the point furthest from the
				center of the given polyhedron, as the smallest sphere might
				not be centered at the centroid.
			*/

			if (vertices.empty()) {
				return;
			}

			// Initializes the center to the first point
			Vector3D center = vertices[0];

			/*
				First we find the center of the bounding sphere which
				may differ from the centre of gravity if we wish the bounding
				sphere to be minimal.
			*/
			for (const Vector3D& point : vertices) {
				center.x += point.x;
				center.y += point.y;
				center.z += point.z;
			}
			center.x /= vertices.size();
			center.y /= vertices.size();
			center.z /= vertices.size();

			// We then calculates its radius
			radius = (real)0.0;
			for (const Vector3D& point : vertices) {
				float distance = realSqrt(
					(point.x - center.x) * (point.x - center.x) +
					(point.y - center.y) * (point.y - center.y) +
					(point.z - center.z) * (point.z - center.z)
				);
				if (distance > radius) {
					radius = distance;
				}
			}

			position = center;
		}


		real getRadius() const {
			return radius;
		}


		/*
			The smallest sphere encompassing a sphere (centered at the
			same place) is the sphere itself.
		*/
		real getBVHSphereRadius() const override {
			return radius;
		}
	};
}

#endif