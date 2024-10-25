
#include "face.h"

using namespace pe;


bool Face::findUniqueVertexIndexes(
	const Mesh* mesh,
	int index[3]
) const {
	std::unordered_set<Vector3D> set;
	int foundCount = 0;

	set.insert(getVertex(mesh, 0));
	index[foundCount] = 0;
	foundCount++;

	for (int i = 1; i < getVertexCount(); i++) {
		if (set.find(getVertex(mesh, i)) == set.end()) {
			set.insert(getVertex(mesh, i));
			index[foundCount] = i;
			foundCount++;

			if (foundCount == 3) {
				// Successfully found 3 unique vertices
				return true;
			}
		}
	}

	// If fewer than 3 unique vertices were found
	return false;
}


Vector3D Face::calculateNormal(const Mesh* mesh) const {

	/*
		Because it is possible that the first three vertices
		(which are used in teh calculation) are not necessarily
		distinct, we ensure that the vertices we use are all distinct.
		If three unique vertices are not found, there is no need
		to have a normal vector as the face is a line or point.
	*/

	int index[3];
	if (!findUniqueVertexIndexes(mesh, index)) {
		return Vector3D::ZERO;
	}

	Vector3D AB = getVertex(mesh, index[1]) - getVertex(mesh, index[0]);
	Vector3D AC = getVertex(mesh, index[2]) - getVertex(mesh, index[0]);

	Vector3D normal = AB.vectorProduct(AC);
	normal.normalize();
	return normal;
}


Vector3D Face::calculateCentroid(const Mesh* mesh) const {

	Vector3D sum;
	for (int i = 0; i < getVertexCount(); i++) {
		sum += getVertex(mesh, i);
	}
	Vector3D centroid = sum * (
		1.0f / static_cast<real>(getVertexCount())
		);
	return centroid;
}


Face::Face(const Mesh* mesh, const std::vector<int>& indexes) :
	indexes{ indexes } {

	update(mesh);

	// Initially, the texture coordinates are all 0
	textureCoordinates = 
		std::vector<Vector2D>(getVertexCount(), Vector2D::ZERO);
}


void Face::update(const Mesh* mesh) {
	centroid = calculateCentroid(mesh);
	normal = calculateNormal(mesh);
}


Vector3D Face::getVertex(const Mesh* mesh, int index) const {
	return mesh->getVertex(indexes[index]);
}


Vector3D Face::getNormal() const {
	return normal;
}


Vector3D Face::getCentroid() const {
	return centroid;
}


int Face::getIndex(int index) const {
	return indexes[index];
}


inline int Face::getVertexCount() const {
	return indexes.size();
}


void Face::setTextureCoordinates(
	const std::vector<Vector2D>& textureCoordinates
) {
	this->textureCoordinates = textureCoordinates;
}


Vector2D Face::getTextureCoordinate(int index) const {
	return textureCoordinates[index];
}


bool Face::containsPoint(const Mesh* mesh, const Vector3D& point, int rayLength) const {

	// Any vertex of the face
	Vector3D vertex0 = getVertex(mesh, 0);

	Vector3D rayEnd(
		point.x + rayLength * normal.x,
		point.y + rayLength * normal.y,
		point.z + rayLength * normal.z
	);

	// Here, we use the method of winding numbers
	int windingNumber = 0;
	for (int i = 0; i < 3; i++) {
		Vector3D edgeStart = getVertex(mesh, i);
		Vector3D edgeEnd = getVertex(mesh, (i + 1) % 3);

		Vector3D edge(
			edgeEnd.x - edgeStart.x,
			edgeEnd.y - edgeStart.y,
			edgeEnd.z - edgeStart.z
		);
		Vector3D pointToStart(
			edgeStart.x - point.x,
			edgeStart.y - point.y,
			edgeStart.z - point.z
		);

		// Checking if the point is below the edge
		if (pointToStart.y > 0 && edge.y >= 0) {
			if (pointToStart.vectorProduct(edge).z > 0) {
				windingNumber++;
			}
		}
		// Checking if the point is above the edge
		else if (pointToStart.y <= 0 && edge.y < 0) {
			if (pointToStart.vectorProduct(edge).z < 0) {
				windingNumber--;
			}
		}
	}

	return windingNumber != 0;

}