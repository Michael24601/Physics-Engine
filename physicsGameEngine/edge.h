/*
	Header file for class representing an edge in a Polyhedron.
*/

#ifndef EDGE_H
#define EDGE_H

#include "vector3D.h"
#include "vector2D.h"
#include "matrix3x4.h"
#include <vector>

namespace pe {

	class Edge {

	private:

		std::vector<Vector3D>* localVertices;
		std::vector<Vector3D>* globalVertices;
		std::pair<int, int> indices;

	public:

		Edge() {}

		Edge(
			std::vector<Vector3D>* localVertices,
			std::vector<Vector3D>* globalVertices,
			int index1,
			int index2
		) : localVertices{ localVertices },
			globalVertices{ globalVertices },
			indices{ std::make_pair(index1, index2) } {}


		Vector3D& getVertex(int index) const {
			if (index == 0) {
				return (*globalVertices)[indices.first];
			}
			else if (index == 1) {
				return (*globalVertices)[indices.second];
			}
			else {
				throw new std::invalid_argument(
					"An edge only has two vertices"
				);
			}
		}
	};
}


#endif