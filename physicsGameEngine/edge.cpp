
#include "edge.h"
#include "mesh.h"

using namespace pe;


inline const Vector3D& Edge::getVertex(const Mesh* mesh, int index) const {
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


inline int Edge::getIndex(int index) const {
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


Vector3D Edge::getMidpoint(Mesh* mesh) const {
	return (getVertex(mesh, 0) + getVertex(mesh, 1)) * 0.5;
}