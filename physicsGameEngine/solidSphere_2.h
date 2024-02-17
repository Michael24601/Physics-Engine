
#ifndef SOLID_SPHERE_H
#define SOLID_SPHERE_H

#include "polyhedron_2.h"

namespace pe {

	class SolidSphere : public Polyhedron {

	private:

		// Tesselates sphere by generating vertices
		static std::vector<Vector3D> generateSphereVertices(
			const Vector3D& center,
			real radius,
			int latitudeSegments,
			int longitudeSegments
		) {

			std::vector<Vector3D> vertices;
			for (int lat = 0; lat <= latitudeSegments; ++lat) {
				for (int lon = 0; lon <= longitudeSegments; ++lon) {
					// Vertices of the current square
					Vector3D v;

					real phi = 2.0f * PI * static_cast<real>(lon)
						/ static_cast<real>(longitudeSegments);
					real theta = PI * static_cast<real>(lat)
						/ static_cast<real>(latitudeSegments);

					// Calculate the vertex
					v += Vector3D(
						sin(theta) * cos(phi),
						cos(theta),
						sin(theta) * sin(phi)
					);
					v.normalize();
					v *= radius;
					v += center;

					vertices.push_back(v);
				}
			}
			return vertices;
		}


		static std::vector<std::vector<int>> generateFaceIndexes(
			int latitudeSegments,
			int longitudeSegments
		) {

			std::vector<std::vector<int>> faceIndexes;

			// Generates sphere faces based on how the vertices were
			for (int lat = 0; lat < latitudeSegments; lat++) {
				for (int lon = 0; lon < longitudeSegments; lon++) {
					/*
						Indices of the four vertices of the current triangle
						in counterclockwise order.
					*/
					int v0 = lat * (longitudeSegments + 1) + lon;
					int v1 = v0 + 1;
					int v2 = (lat + 1) * (longitudeSegments + 1) + lon;
					int v3 = v2 + 1;

					// Forms face in counter-clockwise order
					std::vector<int> indexes{ v0, v1, v3, v2 };
					faceIndexes.push_back(indexes);
				}
			}

			return faceIndexes;
		}


		static std::vector<std::pair<int, int>> generateEdgeIndexes(
			int latitudeSegments,
			int longitudeSegments
		) {

			std::vector<std::pair<int, int>> edgeIndexes;

			// Generates sphere edges based on how the vertices were
			for (int lat = 0; lat < latitudeSegments; lat++) {
				for (int lon = 0; lon < longitudeSegments; lon++) {
					/*
						Indices of the four vertices of the current square
						in counter clockwise order
					*/
					int v0 = lat * (longitudeSegments + 1) + lon;
					int v1 = v0 + 1;
					int v2 = (lat + 1) * (longitudeSegments + 1) + lon;
					int v3 = v2 + 1;

					edgeIndexes.push_back(std::make_pair(v0, v2));
					edgeIndexes.push_back(std::make_pair(v2, v1));
					edgeIndexes.push_back(std::make_pair(v2, v3));
					edgeIndexes.push_back(std::make_pair(v1, v0));
				}
			}

			return edgeIndexes;
		}

	public:

		real radius;
		int latitudeSegments;
		int longitudeSegments;

		SolidSphere(
			RigidBody* body,
			real radius,
			real mass,
			int latitudeSegments,
			int longtitudeSegments,
			Vector3D position) :
			Polyhedron(
				body,
				mass,
				position,
				Matrix3x3(
					(2.0 / 5.0)* mass* radius* radius, 0, 0,
					0, (2.0 / 5.0)* mass* radius* radius, 0,
					0, 0, (2.0 / 5.0)* mass* radius* radius
				),
				generateSphereVertices(
					Vector3D(0, 0, 0),
					radius,
					latitudeSegments,
					longtitudeSegments
				),
				generateFaceIndexes(latitudeSegments, longitudeSegments),
				generateEdgeIndexes(latitudeSegments, longitudeSegments)
			),
			radius{ radius }, latitudeSegments{ latitudeSegments },
			longitudeSegments{ longtitudeSegments } {}

	};
}


#endif