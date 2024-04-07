
#include "polyhedron.h"

using namespace pe;

void  pe::Polyhedron::calculateBoundingBox(
	const std::vector<Vector3D>& vertices,
	Vector3D& halfsize,
	Vector3D& offset
) {
	if (vertices.empty()) {
		return;
	}

	// Initialize min and max coordinates with the first vertex
	Vector3D minCoord = vertices[0];
	Vector3D maxCoord = vertices[0];

	// Iterate through all vertices to find the minimum and maximum coordinates
	for (const Vector3D& vertex : vertices) {
		minCoord.x = std::min(minCoord.x, vertex.x);
		minCoord.y = std::min(minCoord.y, vertex.y);
		minCoord.z = std::min(minCoord.z, vertex.z);

		maxCoord.x = std::max(maxCoord.x, vertex.x);
		maxCoord.y = std::max(maxCoord.y, vertex.y);
		maxCoord.z = std::max(maxCoord.z, vertex.z);
	}

	// Calculate halfsize by taking half of the distance between min and max coordinates
	halfsize = (maxCoord - minCoord) * 0.5;

	/*
		The bounding box centre may not be the same as the centre of gravity
		of the shape; so we need an offset to the centre of the box.
		The offset will be equal to offset - centre of gravity = offset - 0
		as the centre of gravity is 0 in local coordinates.
	*/
	offset = (maxCoord + minCoord) * 0.5;
}


void Polyhedron::calculateBoundingSphere(
	const std::vector<Vector3D>& points,
	real& radius,
	Vector3D& offset
) {
	if (points.empty()) {
		return;
	}

	// Initializes the center to the first point
	Vector3D center = points[0];

	/*
		First we find the center of the bounding sphere which
		may differ from the centre of gravity if we wish the bounding
		sphere to be minimal.
	*/
	for (const Vector3D& point : points) {
		center.x += point.x;
		center.y += point.y;
		center.z += point.z;
	}
	center.x /= points.size();
	center.y /= points.size();
	center.z /= points.size();

	// We then calculates its radius
	radius = (real)0.0;
	for (const Vector3D& point : points) {
		float distance = std::sqrt(
			(point.x - center.x) * (point.x - center.x) +
			(point.y - center.y) * (point.y - center.y) +
			(point.z - center.z) * (point.z - center.z)
		);
		if (distance > radius) {
			radius = distance;
		}
	}

	offset = center;
}


Polyhedron::Polyhedron(
	real mass,
	const Vector3D& position,
	const Matrix3x3& inertiaTensor,
	const std::vector<Vector3D>& localVertices,
	RigidBody* body
) : body{ body },
	localVertices{ localVertices },
	furthestPoint(findFurthestPoint())
{
	body->setMass(mass);
	body->position = position;
	body->setInertiaTensor(inertiaTensor);

	globalVertices = localVertices;

	body->angularDamping = 1;
	body->linearDamping = 1;
	body->calculateDerivedData();

	calculateBoundingBox(localVertices, boundingBoxHalfsize, boundingBoxOffset);
	calculateBoundingSphere(localVertices, boundingSphereRadius, boundingSphereOffset);
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


void Polyhedron::update() {
	globalVertices.clear();
	for (const Vector3D& vertex : localVertices) {
		globalVertices.push_back(
			body->transformMatrix.transform(vertex)
		);
	}

	/*
		Because we use pointers to the vectors of vertices in the
		Polyhedron class in the Faces and Edges, we don't need
		to update them. However, faces also have normals and
		centroids which need to be updated here.
	*/
	for (Face* face : faces) {
		face->update(body->transformMatrix);
	}
}


Vector3D Polyhedron::findFurthestPoint() const {

	Vector3D furthestPoint = *std::max_element(
		localVertices.begin(),
		localVertices.end(),
		[](const Vector3D& first, const Vector3D& second) -> bool {
			return first.magnitudeSquared()
				< second.magnitudeSquared();
		}
	);
	return furthestPoint;
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


Vector3D Polyhedron::support(const Vector3D& direction) const {
	// Initialize with the first vertex
	Vector3D furthestVertex = globalVertices[0];
	real maxDot = furthestVertex.scalarProduct(direction);

	// Iterate through all vertices to find the one farthest in the given direction
	for (size_t i = 1; i < globalVertices.size(); ++i) {
		real dotProduct = globalVertices[i].scalarProduct(direction);
		if (dotProduct > maxDot) {
			maxDot = dotProduct;
			furthestVertex = globalVertices[i];
		}
	}

	return furthestVertex;
}


Vector3D Polyhedron::getHalfsize() const {
	return boundingBoxHalfsize;
}


Vector3D Polyhedron::getOffset() const {
	return boundingBoxOffset;
}

Vector3D Polyhedron::getFurthestPoint() const {
	return furthestPoint;
}


bool isPointInsideFace(
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


bool pe::isPointInsidePolyhedron(
	const Polyhedron& polyhedron,
	const Vector3D& point
) {

	int intersectionCount = 0;

	/*
		We then need to construct a ray from the point to a point outside
		the polyhedron. This was described as projecting the ray to
		infinity. Parctically, what we do to ensure the point ends up
		outside the polyhedron, is find the vertex furthest to the point
		and project the ray from the point at least that far.
	*/
	 real furthestDistance = furthestPoint(
		 point, 
		 polyhedron.globalVertices
	 ).magnitude();

	for (const Face* face : polyhedron.faces) {
		// Here we check if the point is inside the current face
		if (isPointInsideFace(face, point, furthestDistance)) {
			++intersectionCount;
		}
	}

	/*
		If the number of intersections is odd, the point is inside the
		polyhedron.
	*/
	return (intersectionCount % 2) == 1;
}