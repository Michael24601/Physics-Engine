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


		Vector3D getVertex(int index, Basis basis = Basis::GLOBAL) const {

			std::vector<Vector3D>* vertices;
			if (basis == Basis::GLOBAL) {
				vertices = globalVertices;
			}
			else if (basis == Basis::LOCAL) {
				vertices = localVertices;
			}
			else {
				return Vector3D();
			}

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


		int getIndex(int index) {
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
	};
}


#endif