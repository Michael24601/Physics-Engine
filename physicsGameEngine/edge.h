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

	// Forward declaration to avoid circular dependency
	class Mesh;

	class Edge {

	public:

		std::pair<int, int> indexes;

		Edge() {}


		Edge(int index1, int index2) : 
			indexes{ std::make_pair(index1, index2) } {}


		inline const Vector3D& getVertex(const Mesh* mesh, int index) const;


		inline int getIndex(int index) const;


		Vector3D getMidpoint(Mesh* mesh) const;

	};
}


#endif