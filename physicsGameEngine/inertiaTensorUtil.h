/*
	Repository for well known inertia tensors, as well as an inertia
	tensor approximation function.
*/

#ifndef INERTIA_TENSOR_UTIL
#define INERTIA_TENSOR_UTIL

#include "mesh.h"
#include "matrix3x3.h"
#include "axisAlignedBoundingBox.h"

namespace pe {

	class InertiaTensorUtil {

	public:
		
		static Matrix3x3 getRectangularPrismInertiaTensor(
			real width,
			real height,
			real depth,
			real mass
		) {
			return Matrix3x3(
				(mass / 12.0) * (height * height + depth * depth), 0, 0,
				0, (mass / 12.0) * (width * width + depth * depth), 0,
				0, 0, (mass / 12.0) * (width * width + height * height)
			);
		}


		static Matrix3x3 getConeInertiaTensor(
			real height,
			real radius,
			real mass
		) {
			return Matrix3x3(
				(3.0 / 80.0) * mass * (radius * radius + 4.0 * height * height), 0, 0,
				0, (3.0 / 80.0) * mass * (radius * radius + 4.0 * height * height), 0,
				0, 0, (3.0 / 40.0) * mass * radius * radius
			);
		}


		static Matrix3x3 getCylinderInertiaTensor(
			real height,
			real radius,
			real mass
		) {		
			return Matrix3x3(
				(1.0 / 12.0) * mass *
				(3.0 * radius * radius + height * height), 0, 0,
				0, (1.0 / 12.0) * mass *
				(3.0 * radius * radius + height * height), 0,
				0, 0, (1.0 / 12.0) * mass * (3.0 * radius * radius)
			);
		}

		static Matrix3x3 getPyramidInertiaTensor(
			real side,
			real height,
			real mass
		) {
			return Matrix3x3(
				(mass / 10.0) * (3 * side * side + height * height), 0, 0,
				0, (mass / 10.0) * (3 * side * side + height * height), 0,
				0, 0, (mass / 5.0) * side * side
			);
		}


		/*
			The inertia tensor of a solid sphere is different than an empty
			sphere. This is a full sphere.
		*/
		static Matrix3x3 getSolidSphereInertiaTensor(
			real radius,
			real mass
		) {
			return Matrix3x3(
				(2.0 / 5.0) * mass * radius * radius, 0, 0,
				0, (2.0 / 5.0) * mass * radius * radius, 0,
				0, 0, (2.0 / 5.0) * mass * radius * radius
			);
		}


		/*
			This function approximates the inertia tensor of a solid,
			closed mesh.

			Each entry of the inertia tensor is a triple integral over the
			volume of the shape, with the following function being integrated
			I_ij = (mass / volume) * \int_V (x^2 + y^2 + z^2) * delta_ij -r_ir_j dV
			where delat_ij is 1 if i = j, and 0 otherwise, and (x^2 + y^2 + z^2)
			is the magnitude squared of the vector, and r is the vector itself.
			Because it is an integral, we can approximate it using a sum
			I_ij = mass * \sum_k (x^2 + y^2 + z^2) * delta_ij -r_kir_kj
			where k is one point. Done over many points (thousands), this very
			accurately approximates the integral.
			The points will have to be uniformally distributed inside the volume
			of the shape. This means that they will have to be picked at random.
			It's especially easy to pick random points in a rectamgular prism
			aligned with the cartesian plane.
			One startegy will be thus be to choose points at random in the
			bounding box (smallest rectangular prism aligned with the plane
			containing the shape) and taking only those that are in the shape
			(checking if they are in the polyhedron using the monte carlo
			apporach). This works even for convex shapes.
		*/
		static Matrix3x3 approximateMeshInertiaTensor(
			Mesh* mesh,
			int numberOfSamplePoints,
			real mass
		) {

			// First we place the vertices in a box
			AxisAlignedBoundingBox boundingBox;
			boundingBox.fit(mesh->vertices);

			Vector3D offset = boundingBox.getPosition();
			Vector3D halfsize = boundingBox.getHalfsize();

			// Minimum and maximum bounds
			real minX = offset.x - halfsize.x;
			real maxX = offset.x + halfsize.x;
			real minY = offset.y - halfsize.y;
			real maxY = offset.y + halfsize.y;
			real minZ = offset.z - halfsize.z;
			real maxZ = offset.z + halfsize.z;

			std::vector<Vector3D> points;

			/*
				We can generate points in the box, and select the
				points inside the mesh. That is how we sample random points
				in the mesh.
			*/
			const int numPoints = numberOfSamplePoints;
			for (int i = 0; i < numPoints; i++) {
				real randX = generateRandomNumber(minX, maxX);
				real randY = generateRandomNumber(minY, maxY);
				real randZ = generateRandomNumber(minZ, maxZ);

				Vector3D randomPoint(randX, randY, randZ);
				if (mesh->isPointInsideMesh(randomPoint)) {
					points.push_back(randomPoint);
				}
			}

			// The random points are used to approximate the inertia tensor

			real pointMass = mass / (real)numPoints;
			real matrixEntry[3][3]{ {0, 0, 0}, {0, 0, 0}, {0, 0, 0} };
			real sum{ 0 };
			for (const Vector3D& point : points) {
				real distanceSquared = point.magnitudeSquared();

				for (int i = 0; i < 3; i++) {
					for (int j = 0; j < 3; j++) {
						int delta = (i == j ? 1 : 0);

						/*
							Here, point[0] = point.x, point[1] = point.y,
							and point[2] = point.z.
						*/
						matrixEntry[i][j] += pointMass
							* (distanceSquared * delta - point[i] * point[j]);
					}
				}
			}

			return Matrix3x3(
				matrixEntry[0][0], matrixEntry[0][1], matrixEntry[0][2],
				matrixEntry[1][0], matrixEntry[1][1], matrixEntry[1][2],
				matrixEntry[2][0], matrixEntry[2][1], matrixEntry[2][2]
			);
		}

	};
}

#endif