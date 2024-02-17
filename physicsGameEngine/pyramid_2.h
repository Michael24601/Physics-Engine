

#ifndef PYRAMID_H
#define PYRAMID_H

#include "polyhedron_2.h"

namespace pe {

	class Pyramid : public Polyhedron {

	public:

		// Side length
		real side;
		real height;

		Pyramid(
			RigidBody* body, 
			real side, 
			real height, 
			real mass,
			Vector3D position) : 
			Polyhedron(
				body,
				mass,
				position,
				Matrix3x3(
					(mass / 10.0)* (3 * side * side + height * height), 0, 0,
					0, (mass / 10.0)* (3 * side * side + height * height), 0,
					0, 0, (mass / 5.0)* side* side
				),
				std::vector<Vector3D>{
					Vector3D(0, 3 * height / 4.0, 0),
					Vector3D(-side / 2, -height / 4.0, -side / 2),
					Vector3D(side / 2, -height / 4.0, -side / 2),
					Vector3D(side / 2, -height / 4.0, side / 2),
					Vector3D(-side / 2, -height / 4.0, side / 2),
				}
			),
			side{ side }, height{ height } {}


		virtual void setEdges() override {
			edges.resize(8);

			// Define the edges of the pyramid
			edges[0] = Edge(&localVertices, &globalVertices, 1, 2);
			edges[1] = Edge(&localVertices, &globalVertices, 2, 3);
			edges[2] = Edge(&localVertices, &globalVertices, 3, 4);
			edges[3] = Edge(&localVertices, &globalVertices, 4, 1);
			edges[4] = Edge(&localVertices, &globalVertices, 0, 1);
			edges[5] = Edge(&localVertices, &globalVertices, 0, 2);
			edges[6] = Edge(&localVertices, &globalVertices, 0, 3);
			edges[7] = Edge(&localVertices, &globalVertices, 0, 4);
		}

		// All vertices are in clockwise order
		virtual void setFaces() override {
			faces.resize(5);

			// Base face
			faces[0] = Face(
				&localVertices, &globalVertices, std::vector<int>{1, 2, 3, 4}
			);

			// Side faces
			faces[1] = Face(
				&localVertices, &globalVertices, std::vector<int>{0, 1, 4}
			);
			faces[2] = Face(
				&localVertices, &globalVertices, std::vector<int>{0, 4, 3}
			);
			faces[3] = Face(
				&localVertices, &globalVertices, std::vector<int>{0, 3, 2}
			);
			faces[4] = Face(
				&localVertices, &globalVertices, std::vector<int>{0, 2, 1}
			);
		}

	};
}

#endif

