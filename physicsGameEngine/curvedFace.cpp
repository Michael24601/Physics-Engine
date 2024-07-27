
#include "curvedFace.h"

using namespace pe;


Vector3D CurvedFace::calculateTangent(int index) const {

	/*
		We need to find distinct vertices to either side of any
		vertex (first one, 0 in our case), since the tangent
		is uniform across all the face (assuming it's flat).
	*/
	int firstIndex = index, secondIndex, thirdIndex;
	findDistinctVertices(
		firstIndex, secondIndex, thirdIndex
	);

	// Get two edges of the triangle
	Vector3D edge1 = getVertex(secondIndex) -
		getVertex(firstIndex);
	Vector3D edge2 = getVertex(thirdIndex) -
		getVertex(firstIndex);

	// Get the corresponding texture coordinate differences
	Vector2D deltaUV1 = textureCoordinates[secondIndex] -
		textureCoordinates[firstIndex];
	Vector2D deltaUV2 = textureCoordinates[thirdIndex] -
		textureCoordinates[firstIndex];

	// Solve linear equation system to find tangent
	float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
	Vector3D tangent(
		f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x),
		f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y),
		f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z)
	);

	// Orthogonalize the tangent with respect to the vertex normal
	tangent = (
		tangent - (getVertexNormal(firstIndex) *
			(getVertexNormal(firstIndex).scalarProduct(tangent)))
		).normalized();

	return tangent;
}


Vector3D CurvedFace::calculateBitangent(int index) const {

	/*
		We need to find distinct vertices to either side of any
		vertex (first one, 0 in our case), since the bitangent
		is uniform across all the face (assuming it's flat).
	*/
	int firstIndex = index, secondIndex, thirdIndex;
	findDistinctVertices(
		firstIndex, secondIndex, thirdIndex
	);

	// Get two edges of the triangle
	Vector3D edge1 = getVertex(secondIndex) -
		getVertex(firstIndex);
	Vector3D edge2 = getVertex(thirdIndex) -
		getVertex(firstIndex);

	// Get the corresponding texture coordinate differences
	Vector2D deltaUV1 = textureCoordinates[secondIndex] -
		textureCoordinates[firstIndex];
	Vector2D deltaUV2 = textureCoordinates[thirdIndex] -
		textureCoordinates[firstIndex];

	// Solve linear equation system to find bitangent
	float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
	Vector3D bitangent(
		f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x),
		f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y),
		f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z)
	);

	/*
		Orthogonalize the bitangent with respect to the vertex normal
		This step is crucial since the vertex normal ensures the
		tangent and bitangents are different for different vertices,
		otherwise they won't be created orthogonally relative to each
		vertex's normal.
	*/
	bitangent = (
		bitangent - (getVertexNormal(firstIndex) *
			(getVertexNormal(firstIndex).scalarProduct(bitangent)))
		).normalized();

	return bitangent;
}


CurvedFace::CurvedFace(
	std::vector<Vector3D>* vertices,
	std::vector<int>& indices,
	std::vector<Vector3D>& vertexNormals
) : Face(vertices, indices),
	vertexNormals{ vertexNormals } {

	/*
		We then calculate each tangentand bitangent, which are
		also distinct on a curved surface.
	*/
	vertexTangents.resize(getVertexNumber());
	vertexBitangents.resize(getVertexNumber());
	for (int i = 0; i < getVertexNumber(); i++) {
		vertexTangents[i] = calculateTangent(i);
		vertexBitangents[i] = calculateBitangent(i);
	}
}


void CurvedFace::recalculateFrameVectors() {
	Face::recalculateFrameVectors();
	for (int i = 0; i < getVertexNumber(); i++) {
		vertexTangents[i] = calculateTangent(i);
		vertexBitangents[i] = calculateBitangent(i);
	}
}


void CurvedFace::setNormal(int index, const Vector3D& normal) {
	vertexNormals[index] = normal;
}


Vector3D CurvedFace::getVertexNormal(int index) const {
	return vertexNormals[index];
}


Vector3D CurvedFace::getVertexTangent(int index) const {
	return vertexTangents[index];
}


Vector3D CurvedFace::getVertexBitangent(int index) const {
	return vertexBitangents[index];
}


void CurvedFace::setTextureCoordinates(
	std::vector<Vector2D>& textureCoordinates
) {
	this->textureCoordinates = textureCoordinates;

	/*
		We then need to recalculate the tangentand bitangents which depend
		on the texture coordinates.
	*/
	tangent = Face::calculateTangent();
	bitangent = Face::calculateBitangent();

	for (int i = 0; i < getVertexNumber(); i++) {
		vertexTangents[i] = calculateTangent(i);
		vertexBitangents[i] = calculateBitangent(i);
	}
}