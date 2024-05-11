/*
	Header file for class representing an edge in a Polyhedron.
*/

#ifndef EDGE_H
#define EDGE_H

#include "vector3D.h"
#include "vector2D.h"
#include "matrix3x4.h"
#include "face.h"
#include <vector>

namespace pe {

	class Edge {

	private:

		std::vector<Vector3D>* vertices;

	public:

		std::pair<int, int> indices;

		Edge() {}


		Edge(
			std::vector<Vector3D>* vertices,
			int index1,
			int index2
		) : vertices{ vertices },
			indices{ std::make_pair(index1, index2) } {}


		inline Vector3D getVertex(int index) const {
			if (index == 0) {
				return (*vertices)[indices.first];
			}
			else if (index == 1) {
				return (*vertices)[indices.second];
			}
			else {
				throw new std::invalid_argument(
					"An edge only has two vertices"
				);
			}
		}


		inline int getIndex(int index) {
			if (index == 0) {
				return indices.first;
			}
			else if (index == 1) {
				return indices.second;
			}
			else {
				throw new std::invalid_argument(
					"An edge only has two vertices"
				);
			}
		}


		Vector3D getMidpoint() const {
			return (getVertex(0) + getVertex(1)) * 0.5;
		}

	};
}


#endif