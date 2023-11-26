
#include "primitive.h"
#include "sat.h"

using namespace pe;


std::vector<std::pair<Vector3D, Vector3D>> 
	Primitive::getFaceNormals(real length) const {

	std::vector<std::pair<Vector3D, Vector3D>> normals;

	for (const auto& face : this->faces) {
		// Calculates the average of the face vertices (centre)
		Vector3D center = face.centroid();

		// Calculates the endpoint of the normal line
		Vector3D normal = face.normal();
		Vector3D endpoint = center + (normal * length);

		normals.push_back(std::make_pair(center, endpoint));
	}

	return normals;
}


std::vector<std::pair<Vector3D, Vector3D>> Primitive::getEdges() const {
	std::vector<std::pair<Vector3D, Vector3D>> lines;

	for (int i = 0; i < edges.size(); i++) {
		lines.push_back(std::make_pair(*edges[i].vertices[0],
			*edges[i].vertices[1]));
	}

	return lines;
}


std::vector<std::vector<Vector3D>> Primitive::getFaces() const {
	std::vector<std::vector<Vector3D>> vectors;

	for (int i = 0; i < faces.size(); i++) {
		vectors.push_back(std::vector<Vector3D>(0));
		for(int j = 0; j < faces[i].vertices.size(); j++){
			vectors[i].push_back(*faces[i].vertices[j]);
		}
	}

	return vectors;
}


void Primitive::updateVertices() {
	for (int i = 0; i < globalVertices.size(); i++) {
		globalVertices[i] =
			body->transformMatrix.transform(localVertices[i]);
	}
}


bool Primitive::isColliding(const Primitive& primitive) const {
	return testIntersection(*this, primitive);
}