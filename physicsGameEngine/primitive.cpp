
#include "primitive.h"
#include "sat.h"

using namespace pe;


std::vector<std::pair<Vector3D, Vector3D>> 
	Primitive::calculateFaceNormals(real length) const {

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


std::vector<std::pair<Vector3D, Vector3D>> Primitive::drawLines() const {
	std::vector<std::pair<Vector3D, Vector3D>> lines;

	for (int i = 0; i < edges.size(); i++) {
		lines.push_back(std::make_pair(*edges[i].vertices[0],
			*edges[i].vertices[1]));
	}

	return lines;
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