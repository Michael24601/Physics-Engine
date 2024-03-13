
#include "face.h"

using namespace pe;

void Face::findDistinctVertices(
	int givenIndex,
	int& firstIndex,
	int& secondIndex
) const {

	// Counter-clockwise side
	for (
		int i = (givenIndex + 1) % getVertexNumber();
		i != givenIndex;
		i = (i + 1) % getVertexNumber()
		) {
		if (
			getVertex(i, Basis::LOCAL) !=
			getVertex(givenIndex, Basis::LOCAL)
			) {
			firstIndex = i;
			break;
		}
	}

	// Clockwise side
	for (
		int i = (givenIndex == 0 ? getVertexNumber() - 1 : givenIndex - 1);
		i != givenIndex;
		// Loops around
		i = (i == 0 ? getVertexNumber() - 1 : (i - 1) % getVertexNumber())
		) {
		if (
			getVertex(i, Basis::LOCAL) !=
			getVertex(givenIndex, Basis::LOCAL)
			) {
			secondIndex = i;
			break;
		}
	}
}


Vector3D Face::calculateNormal(Basis basis) const {

	/*
		We need to find distinct vertices to either side of any
		vertex (first one, 0 in our case), since the normal
		is uniform across all the face (assuming it's flat).
	*/
	int firstIndex = 0, secondIndex, thirdIndex;
	findDistinctVertices(
		firstIndex, secondIndex, thirdIndex
	);

	Vector3D AB = getVertex(secondIndex, basis) -
		getVertex(firstIndex, basis);
	Vector3D AC = getVertex(thirdIndex, basis) -
		getVertex(firstIndex, basis);

	Vector3D normal = AB.vectorProduct(AC);
	normal.normalize();
	return normal;
}


Vector3D Face::calculateCentroid(Basis basis) const {

	Vector3D sum;
	for (int i = 0; i < getVertexNumber(); i++) {
		sum += getVertex(i, basis);
	}
	Vector3D centroid = sum * (
		1.0f / static_cast<real>(getVertexNumber())
		);
	return centroid;
}


Vector3D Face::calculateTangent(Basis basis) const {

	/*
		We need to find distinct vertices to either side of any
		vertex (first one, 0 in our case), since the tangent
		is uniform across all the face (assuming it's flat).
	*/
	int firstIndex = 0, secondIndex, thirdIndex;
	findDistinctVertices(
		firstIndex, secondIndex, thirdIndex
	);

	// Get two edges of the triangle
	Vector3D edge1 = getVertex(secondIndex, basis) -
		getVertex(firstIndex, basis);
	Vector3D edge2 = getVertex(thirdIndex, basis) -
		getVertex(firstIndex, basis);

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

	/*
		Orthogonalize the tangent with respect to the face normal
		(since this is flat face and all vertices have a uniform
		normal.
	*/
	tangent = (tangent - (
		getNormal(basis) * (getNormal(basis).scalarProduct(tangent))
		)).normalized();

	return tangent;
}


Vector3D Face::calculateBitangent(Basis basis) const {

	/*
		We need to find distinct vertices to either side of any
		vertex (first one, 0 in our case), since the bitangent
		is uniform across all the face (assuming it's flat).
	*/
	int firstIndex = 0, secondIndex, thirdIndex;
	findDistinctVertices(
		firstIndex, secondIndex, thirdIndex
	);

	// Get two edges of the triangle
	Vector3D edge1 = getVertex(secondIndex, basis) -
		getVertex(firstIndex, basis);
	Vector3D edge2 = getVertex(thirdIndex, basis) -
		getVertex(firstIndex, basis);

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
		Orthogonalize the bitangent with respect to the face normal
		(since this is flat face and all vertices have a uniform
		normal.
	*/
	bitangent = (bitangent - (
		getNormal(basis) * (getNormal(basis).scalarProduct(bitangent))
		)).normalized();

	return bitangent;
}


int Face::getFurthestPoint() const {
	// Finds the furthest point
	int maxIndex{ 0 };
	real maxMagnitudeSquared{ 0 };
	for (int i = 0; i < getVertexNumber(); i++) {
		Vector3D d = getVertex(i, Basis::LOCAL) - localCentroid;
		/*
			We can use the magnitude squared as we only care about
			the relative distance, and it is more efficient.
		*/
		real distance = d.magnitudeSquared();
		if (distance > maxMagnitudeSquared) {
			maxMagnitudeSquared = distance;
			maxIndex = i;
		}
	}
	return maxIndex;
}


std::vector<Vector2D> Face::setUvCoordinates() const {

	std::vector<Vector2D> uvCoordinates;

	/*
		First we find the index of the furthest point from the
		centroid.
	*/
	int furthestIndex = getFurthestPoint();

	/*
		First, we need to find the transformation matrix that takes
		the face, and places it in the plane with z = 0 (in 3D space).
		Usually, when changing the basis from the standard basis to
		some other basis, we need to find the basis vectors
		i hat, j hat, and k hat, place their x y and z coordinates
		in a matrix column-wise, and multiply any point by the
		matrix to get it's position in the new basis.
		We need to do the opposite, and transform points from the
		face basis to the standard basis, so we find the described
		matrix and use its inverse.
		Now, in order to find the basis vectors of the basis the
		face belongs to, we can actually just use the tangent,
		bitangent, and normal vectors, with the normal being the z
		axis as it juts out of the face similarly to how we want it
		when we map it to the standard basis.

		Note that linear transformations require that he basis
		vectors stem from the origin, so we need the centroid to be
		at the origin, meaning that we need to first shift
		all vertices, using the same vector -centroid.
	*/
	Vector3D firstShift = localCentroid * -1;
	Matrix3x3 faceToStandard(
		localBitangent.x, localTangent.x, localNormal.x,
		localBitangent.y, localTangent.y, localNormal.y,
		localBitangent.z, localTangent.z, localNormal.z
	);
	faceToStandard.invert();

	/*
		Now we need to scale the point so that they fit in the
		(-0.5, -0.5) to (0.5, 0.5) square. If we take the closest
		point and scale the distance to it so that it is c units
		away from the origin, then the rest of the points will
		fall on the disk of radius c. The only way to guarantee
		the disk all belongs to the square is to have c be 0.5
		(which maximizes the area too).

		Afterwards, we can shift the whole thing by (0.5, 0.5, 0)
		in order to place he vertices in the (0, 0) to (1, 1) square.
	*/
	Vector3D furthestPoint = getVertex(furthestIndex, Basis::LOCAL);
	real distance = furthestPoint.magnitude();
	real scalingFactor = 0.5 / distance;

	Vector3D secondShift(0.5, 0.5, 0);

	/*
		Then we transform each vertex using this technique.
		Note that it doesn't matter wether we use the global
		or local basis and transform it to the standard basis,
		so long as we are consistent about it (using local vertex
		means using local normal and tangent etc...).
	*/
	for (int i = 0; i < getVertexNumber(); i++) {
		Vector3D vertex = getVertex(i, Basis::LOCAL);

		vertex += firstShift;
		vertex = faceToStandard.transform(vertex);
		vertex *= scalingFactor;
		vertex += secondShift;

		uvCoordinates.push_back(
			Vector2D(vertex.x, vertex.y)
		);
	}

	return uvCoordinates;
}


Face::Face(
	std::vector<Vector3D>* localVertices,
	std::vector<Vector3D>* globalVertices,
	std::vector<int>& indeces
) : localVertices{ localVertices },
globalVertices{ globalVertices },
indeces{ indeces } {

	localNormal = calculateNormal(Basis::LOCAL);
	localCentroid = calculateCentroid(Basis::LOCAL);

	/*
		Initially, the normal and centroid are the same as the local
		ones.
	*/
	normal = localNormal;
	centroid = localCentroid;

	/*
		By default the texture coordinates are uniform for the face.
		They are set in counter-clockwise order.
	*/
	textureCoordinates = setUvCoordinates();


	localTangent = calculateTangent(Basis::LOCAL);
	localBitangent = calculateBitangent(Basis::LOCAL);

	tangent = localTangent; // Initially
	bitangent = localBitangent; // Inititally
}


/*
	Returns the local or global vertex at a certain index.
	The basis is global by default.
*/
Vector3D Face::getVertex(int index, Basis basis) const {
	if (basis == Basis::LOCAL) {
		return (*localVertices)[indeces[index]];
	}
	else if (basis == Basis::GLOBAL) {
		return (*globalVertices)[indeces[index]];
	}
}


Vector3D Face::getNormal(Basis basis) const {
	if (basis == Basis::LOCAL) {
		return localNormal;
	}
	else if (basis == Basis::GLOBAL) {
		return normal;
	}
}


Vector3D Face::getCentroid(Basis basis) const {
	if (basis == Basis::LOCAL) {
		return localCentroid;
	}
	else if (basis == Basis::GLOBAL) {
		return centroid;
	}
}


Vector3D Face::getTangent(Basis basis) const {
	if (basis == Basis::LOCAL) {
		return localTangent;
	}
	else if (basis == Basis::GLOBAL) {
		return tangent;
	}
}


Vector3D Face::getBitangent(Basis basis) const {
	if (basis == Basis::LOCAL) {
		return localBitangent;
	}
	else if (basis == Basis::GLOBAL) {
		return bitangent;
	}
}


Vector3D Face::getVertexNormal(
	int index, Basis basis
) const {
	if (basis == Basis::LOCAL) {
		return localNormal;
	}
	else if (basis == Basis::GLOBAL) {
		return normal;
	}
}


Vector3D Face::getVertexTangent(
	int index, Basis basis
) const {
	if (basis == Basis::LOCAL) {
		return localTangent;
	}
	else if (basis == Basis::GLOBAL) {
		return tangent;
	}
}


Vector3D Face::getVertexBitangent(
	int index, Basis basis
) const {
	if (basis == Basis::LOCAL) {
		return localTangent;
	}
	else if (basis == Basis::GLOBAL) {
		return tangent;
	}
}


inline int Face::getVertexNumber() const {
	return indeces.size();
}


std::vector<Vector3D> Face::getVertices() {
	std::vector<Vector3D> vertices(getVertexNumber());
	for (int i = 0; i < getVertexNumber(); i++) {
		vertices[i] = (*globalVertices)[indeces[i]];
	}
	return vertices;
}


void Face::update(const Matrix3x4& transformMatrix) {

	/*
		To transform a normal using a trasnform matrix, it's not
		enough to just multiply it by the matrix; that just gives
		us the point in space the normal will point to. To get the
		normal direction, we need the vector pointing from
		the transformed origin (center) to the afore mentioned point.

		Same for the tangent and bitangent.
	*/
	Vector3D centre = transformMatrix.transform(Vector3D(0, 0, 0));

	Vector3D transformedNormal = transformMatrix.transform(localNormal);
	normal = transformedNormal - centre;
	normal.normalize();

	Vector3D transformedTangent = transformMatrix.transform(localTangent);
	tangent = transformedTangent - centre;
	tangent.normalize();

	Vector3D transformedBitangent = transformMatrix.transform(localBitangent);
	bitangent = transformedBitangent - centre;
	bitangent.normalize();

	centroid = transformMatrix.transform(localCentroid);
}


void Face::setTextureCoordinates(
	std::vector<Vector2D>& textureCoordinates
) {
	this->textureCoordinates = textureCoordinates;
}


Vector2D Face::getTextureCoordinate(int index) const {
	return textureCoordinates[index];
}


int Face::getIndex(int index) {
	return indeces[index];
}


bool Face::containsPoint(const Vector3D& point) const{

	int windingNumber = 0;
	size_t numVertices = getVertexNumber();

	for (size_t i = 0; i < numVertices; ++i) {
		const Vector3D& currentVertex = getVertex(i);
		const Vector3D& nextVertex = getVertex((i + 1) % numVertices);

		if ((currentVertex.y <= point.y && nextVertex.y > point.y) ||
			(currentVertex.y > point.y && nextVertex.y <= point.y)) {

			double intersectionX = (point.y - currentVertex.y) *
				(nextVertex.x - currentVertex.x) /
				(nextVertex.y - currentVertex.y) + currentVertex.x;

			if (point.x < intersectionX) {
				++windingNumber;
			}
		}
	}
	return windingNumber % 2 != 0;
}
