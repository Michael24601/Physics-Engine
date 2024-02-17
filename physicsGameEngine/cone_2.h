

#ifndef CONE_H
#define CONE_H

#include "polyhedron_2.h"

namespace pe {

	class Cone : public Polyhedron {

	private:

		/*
			Defines the vertices of a tessalated cone with a certain radius,
			height, and a number of segments (the more segments the more
			it will look like a cone). 
		*/
		static std::vector<Vector3D> generateConeVertices(
			const Vector3D& center,
			real radius, 
			real height, 
			int segments
		) {

			std::vector<Vector3D> vertices;

			// Adds the apex of the cone at the top
			vertices.push_back(center + Vector3D(0.0f, height / 2.0f, 0.0f));

			for (int i = 0; i <= segments; ++i) {
				// Calculate the angle for each segment
				real theta = 2.0f * PI * static_cast<real>(i) / 
					static_cast<real>(segments);

				// Vertices of the current point on the base of the cone
				Vector3D v_base = center + Vector3D(
					radius * cos(theta),
					-height / 2.0f,
					radius * sin(theta)
				);

				vertices.push_back(v_base);
			}

			return vertices;
		}


		/*
			Generates cone face associations based on the number of
			tesselation segments.
		*/
		static std::vector<std::vector<int>> generateFaceIndexes(
			int segments
		) {

			std::vector<std::vector<int>> faceIndexes;

			// Base face
			std::vector<int> baseFaceIndexes;
			for (int i = 1; i <= segments; i++) {
				baseFaceIndexes.push_back(i);
			}
			faceIndexes.push_back(baseFaceIndexes);

			// Side faces
			for (int i = 1; i <= segments; ++i) {
				int v0 = i;
				int v1 = (i % segments) + 1;

				std::vector<int> sideFaceIndexes = {
					0, // Apex
					v1,
					v0
				};

				faceIndexes.push_back(sideFaceIndexes);
			}

			return faceIndexes;
		}


		static std::vector<std::pair<int, int>> generateEdgeIndexes(
			int segments
		) {

			std::vector<std::pair<int, int>> edgeIndexes;

			// Base edges
			for (int i = 1; i <= segments; ++i) {
				// Indexes of the two vertices of the current edge
				int v0 = i;
				int v1 = (i % segments) + 1;

				// Connects vertices to form edges based on the side faces
				edgeIndexes.push_back(std::make_pair(v0, v1));
			}

			// Edges from the apex to the base vertices
			for (int i = 1; i <= segments; ++i) {
				int v0 = 0;
				int v1 = i;
				edgeIndexes.push_back(std::make_pair(v0, v1));
			}

			return edgeIndexes;
		}

	public:

		real radius;
		real length;
		int segments;

		Cone(
			RigidBody* body,
			real radius,
			real length,
			real mass,
			int segments,
			Vector3D position) :
			Polyhedron(
				body,
				mass,
				position,
				Matrix3x3(
					(3.0 / 80.0)* mass* (radius* radius + 4.0 * length * length), 0, 0,
					0, (3.0 / 80.0)* mass* (radius* radius + 4.0 * length * length), 0,
					0, 0, (3.0 / 40.0)* mass* radius* radius
				),
				generateConeVertices(
					Vector3D(0, 0, 0),
					radius,
					length,
					segments
				),
				generateFaceIndexes(segments),
				generateEdgeIndexes(segments)
			),
			radius{ radius }, length{ length }, segments{ segments } {}

	};
}


#endif