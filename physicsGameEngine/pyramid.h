

#ifndef PYRAMID_H
#define PYRAMID_H

#include "polyhedron.h"

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
			side{ side }, height{ height } {
			
			setEdges();
			setFaces();
		}


		virtual void setEdges() override {
			edges.resize(8);

			// Define the edges of the pyramid
			edges[0] = new Edge(&localVertices, &globalVertices, 1, 2);
			edges[1] = new Edge(&localVertices, &globalVertices, 2, 3);
			edges[2] = new Edge(&localVertices, &globalVertices, 3, 4);
			edges[3] = new Edge(&localVertices, &globalVertices, 4, 1);
			edges[4] = new Edge(&localVertices, &globalVertices, 0, 1);
			edges[5] = new Edge(&localVertices, &globalVertices, 0, 2);
			edges[6] = new Edge(&localVertices, &globalVertices, 0, 3);
			edges[7] = new Edge(&localVertices, &globalVertices, 0, 4);
		}

		// All vertices are in clockwise order
		virtual void setFaces() override {
			faces.resize(5);

			std::vector<std::vector<int>> indexes{
				std::vector<int>{ 1, 2, 3, 4 },
				std::vector<int>{0, 1, 4},
				std::vector<int>{0, 4, 3},
				std::vector<int>{0, 3, 2},
				std::vector<int>{0, 2, 1}
			};

			for (int i = 0; i < indexes.size(); i++) {
				faces[i] = new Face(
					&localVertices, 
					&globalVertices, 
					indexes[i]
				);
			}
		}

	};
}

#endif

