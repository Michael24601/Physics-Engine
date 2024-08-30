

#ifndef PYRAMID_H
#define PYRAMID_H

#include "polyhedron.h"

namespace pe {

	class Pyramid : public Cuboidal {

	private:

		static std::vector<Edge*> generateEdges(
			std::vector<Vector3D>& localVertices
		) {
			std::vector<Edge*> edges(8);

			// Define the edges of the pyramid
			edges[0] = new Edge(&localVertices, 1, 2);
			edges[1] = new Edge(&localVertices, 2, 3);
			edges[2] = new Edge(&localVertices, 3, 4);
			edges[3] = new Edge(&localVertices, 4, 1);
			edges[4] = new Edge(&localVertices, 0, 1);
			edges[5] = new Edge(&localVertices, 0, 2);
			edges[6] = new Edge(&localVertices, 0, 3);
			edges[7] = new Edge(&localVertices, 0, 4);

			return edges;
		}


		// All vertices are in clockwise order
		static std::vector<Face*> generateFaces(
			std::vector<Vector3D>& localVertices
		) {
			std::vector<Face*> faces(5);

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
					indexes[i]
				);
			}

			return faces;
		}


	public:

		// Side length
		real side;
		real height;

		Pyramid(
			real side, 
			real height, 
			real mass,
			Vector3D position,
			RigidBody* body
		) : 
			Cuboidal(
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
				},
				body	
			),
			side{ side }, height{ height } {

			setFaces(generateFaces(localVertices));
			setEdges(generateEdges(localVertices));
		}
		
		Face* getFace(int index) const {
			return faces[index];
		}


		Edge* getEdge(int index) const {
			return edges[index];
		}

	};
}

#endif

