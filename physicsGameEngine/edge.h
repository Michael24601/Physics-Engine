/*
	Header file for class representing an edge in a mesh.
*/

#ifndef EDGE_H
#define EDGE_H

#include "vector3D.h"
#include "vector2D.h"
#include "face.h"
#include <vector>

namespace pe {

	class Edge {

	private:

		Mesh* mesh;

	public:

		std::pair<int, int> indexes;

		Edge() {}


		Edge(Mesh* mesh, int index1, int index2) : 
			mesh{ mesh }, indexes{ std::make_pair(index1, index2) } {}


		inline const Vector3D& getVertex(const Mesh* mesh, int index) const {
			if (index == 0) {
				return mesh->getVertex(indexes.first);
			}
			else if (index == 1) {
				return mesh->getVertex(indexes.second);
			}
			else {
				throw new std::invalid_argument(
					"An edge only has two vertices"
				);
			}
		}


		inline int getIndex(int index) const {
			if (index == 0) {
				return indexes.first;
			}
			else if (index == 1) {
				return indexes.second;
			}
			else {
				throw new std::invalid_argument(
					"An edge only has two vertices"
				);
			}
		}


		Vector3D getMidpoint(Mesh* mesh) const {
			return (getVertex(mesh, 0) + getVertex(mesh, 1)) * 0.5;
		}

	};
}


#endif