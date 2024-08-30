
#include "polyhedron.h"

using namespace pe;


Polyhedron::Polyhedron(
	real mass,
	const Vector3D& position,
	const Matrix3x3& inertiaTensor,
	const std::vector<Vector3D>& localVertices,
	RigidBody* body
) : body{ body },
	localVertices{ localVertices } {

	body->setMass(mass);
	body->position = position;
	body->setInertiaTensor(inertiaTensor);

	body->angularDamping = 1;
	body->linearDamping = 1;
	body->calculateDerivedData();
}


Polyhedron::~Polyhedron() {
	delete body;
	for (int i = 0; i < faces.size(); i++) {
		delete faces[i];
	}
	for (int i = 0; i < edges.size(); i++) {
		delete edges[i];
	}
}


static Vector3D calculateCentreOfGravity(
	const std::vector<Vector3D>& vertices
) {
	Vector3D centre;
	for (const auto& vec : vertices) {
		centre.x += vec.x;
		centre.y += vec.y;
		centre.z += vec.z;
	}
	real numVectors = static_cast<real>(vertices.size());
	centre *= 1.0 / numVectors;
	return centre;
}


void Polyhedron::setFaces(std::vector<Face*> faces) {
	this->faces = faces;
}


void Polyhedron::setEdges(std::vector<Edge*> edges) {
	this->edges = edges;
}


Vector3D Polyhedron::getAxis(int index) const {
	return body->transformMatrix.getColumnVector(index);
}


Vector3D Polyhedron::getCentre() const {
	return body->position;
}


Vector3D Polyhedron::getFaceNormal(int index) const {
	return faces[index]->getNormal();
}


bool Polyhedron::isPointInsideFace(
	const Face* face, 
	const Vector3D& point,
	real rayLength
) {
	// Get the normal vector of the face
	Vector3D normal = face->getNormal();

	// Get a vertex of the face
	Vector3D vertex0 = face->getVertex(0);

	Vector3D rayEnd(
		point.x + rayLength * normal.x, 
		point.y + rayLength * normal.y,
		point.z + rayLength * normal.z
	);

	// Here, we use the method of winding numbers
	int windingNumber = 0;
	for (int i = 0; i < 3; i++) {
		Vector3D edgeStart = face->getVertex(i);
		Vector3D edgeEnd = face->getVertex((i + 1) % 3);

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


const Matrix3x4& Polyhedron::getTransformMatrix() const {
	return body->transformMatrix;
}



bool Polyhedron::isPointInsidePolyhedron(
	const Vector3D& point
) const {

	Vector3D localPoint = getTransformMatrix().inverseTransform(point);

	int intersectionCount = 0;

	/*
		We then need to construct a ray from the point to a point outside
		the polyhedron. This was described as projecting the ray to
		infinity. Parctically, what we do to ensure the point ends up
		outside the polyhedron, is find the vertex furthest to the point
		and project the ray from the point at least that far.
	*/
	 real furthestDistance = findFurthestPointFromCoordinate(
		 localPoint, 
		 this->localVertices
	 ).magnitude();

	for (const Face* face : this->faces) {
		// Here we check if the point is inside the current face
		if (isPointInsideFace(face, localPoint, furthestDistance)) {
			++intersectionCount;
		}
	}

	/*
		If the number of intersections is odd, the point is inside the
		polyhedron.
	*/
	return (intersectionCount % 2) == 1;
}