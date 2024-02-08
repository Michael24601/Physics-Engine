/*

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
		std::pair<int, int> indeces;

	public:

		Edge() {}

		Edge(
			std::vector<Vector3D>* localVertices,
			std::vector<Vector3D>* globalVertices,
			std::pair<int, int> indeces
		) : localVertices{ localVertices },
			globalVertices{ globalVertices },
			indeces{ indeces } {}


		Vector3D& getVertex(int index) const {
			if (index == 0) {
				return (*globalVertices)[indeces.first];
			}
			else if (index == 1) {
				return (*globalVertices)[indeces.second];
			}
			else {
				throw new std::invalid_argument(
					"An edge only has two edges"
				);
			}
		}
	};
}


#endif