
#include "primitive.h"
#include "sat.h"

using namespace pe;


std::vector<std::pair<Vector3D, Vector3D>> 
	Primitive::calculateFaceNormals(real length) const {

	std::vector<std::pair<Vector3D, Vector3D>> normals;

	for (const auto& face : this->faces) {
		// Calculate the average of the face vertices (center)
		Vector3D center = face.centroid();

		// Calculate the endpoint of the normal line
		Vector3D normal = face.normal();
		Vector3D endpoint = center + (normal * length);

		normals.push_back(std::make_pair(center, endpoint));
	}

	return normals;
}


sf::VertexArray Primitive::drawLine(Vector3D* c1, Vector3D* c2,
	sf::Color color) const {
	sf::VertexArray line(sf::LineStrip, 2);
	line[0].position = sf::Vector2f(c1->x, c1->y);
	line[1].position = sf::Vector2f(c2->x, c2->y);
	line[0].color = line[1].color = color;

	return line;
}


std::vector<sf::VertexArray> Primitive::drawLines() const {
	std::vector<sf::VertexArray> lines;

	for (int i = 0; i < edges.size(); i++) {
		lines.push_back(drawLine(edges[i].vertices[0],
			edges[i].vertices[1], sf::Color::White));
	}

	// Also draws the normals
	std::vector<std::pair<Vector3D, Vector3D>> normals
		= calculateFaceNormals(40);
	for (int i = 0; i < normals.size(); i++) {
		lines.push_back(drawLine(&normals[i].first,
			&normals[i].second, sf::Color::Red));
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