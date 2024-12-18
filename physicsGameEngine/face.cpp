
#include "face.h"
#include "mesh.h"

using namespace pe;

bool Face::findUniqueVertexIndexes(
	const Mesh* mesh,
	int index[3]
) const {
	// Initialize the first unique vertex index
	index[0] = 0;
	// Starting with one unique vertex found
	int foundCount = 1;

	for (int i = 1; i < getVertexCount(); i++) {
		bool isUnique = true;

		for (int j = 0; j < foundCount; j++) {
			if (getVertex(mesh, i) == getVertex(mesh, index[j])) {
				isUnique = false; // Not unique if a match is found
				break;
			}
		}

		// If it's unique, we add it to the list
		if (isUnique) {
			index[foundCount] = i;
			foundCount++;

			// We can stop once we've found 3 unique vertices
			if (foundCount == 3) {
				return true;
			}
		}
	}

	// If fewer than 3 unique vertices were found
	return false;
}


void Face::calculateNormal(const Mesh* mesh) {

	/*
		Because it is possible that the first three vertices
		(which are used in teh calculation) are not necessarily
		distinct, we ensure that the vertices we use are all distinct.
		If three unique vertices are not found, there is no need
		to have a normal vector as the face is a line or point.
	*/

	int index[3];
	if (!findUniqueVertexIndexes(mesh, index)) {
		return;
	}

	Vector3D AB = getVertex(mesh, index[1]) - getVertex(mesh, index[0]);
	Vector3D AC = getVertex(mesh, index[2]) - getVertex(mesh, index[0]);

	normal = AB.vectorProduct(AC);
	normal.normalize();
}


void Face::calculateCentroid(const Mesh* mesh) {

	Vector3D sum;
	for (int i = 0; i < getVertexCount(); i++) {
		sum += getVertex(mesh, i);
	}
	centroid = sum * (
		1.0f / static_cast<real>(getVertexCount())
	);
}


Face::Face(const Mesh* mesh, const std::vector<int>& indexes) :
	indexes{ indexes } {

	calculateCentroid(mesh);
	calculateNormal(mesh);

	// Initially, the texture coordinates are all 0
	textureCoordinates = 
		std::vector<Vector2D>(getVertexCount(), Vector2D::ZERO);
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