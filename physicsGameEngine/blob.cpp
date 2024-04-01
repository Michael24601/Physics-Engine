
#include "blob.h"

using namespace pe;


Blob::Blob(
	Vector3D position,
	real radius,
	int rowSize,
	int columnSize,
	real mass,
	real damping,
	real springStrength
) :
	Mesh(
		returnParticleGrid(
			rowSize,
			columnSize,
			position,
			radius
		),
		mass,
		damping
	),
	rowSize{ rowSize },
	columnSize{ columnSize },
	springStrength{ springStrength },
	radius{radius}{

	setFaces(calculateFaces());
	setEdges(calculateEdges());

	setForces();
	setConstraints();
}


std::vector<pe::Vector3D>  Blob::returnParticleGrid(
	int rowSize,
	int columnSize,
	const Vector3D& centre,
	real radius
) {

	std::vector<Vector3D> vertices;

	for (int lat = 0; lat <= rowSize; ++lat) {
		for (int lon = 0; lon <= columnSize; ++lon) {
			// Vertices of the current square
			Vector3D v;

			real phi = 2.0f * PI * static_cast<real>(lon)
				/ static_cast<real>(columnSize);
			real theta = PI * static_cast<real>(lat)
				/ static_cast<real>(rowSize);

			// Calculate the vertex
			v += Vector3D(
				sin(theta) * cos(phi),
				cos(theta),
				sin(theta) * sin(phi)
			);
			v.normalize();
			v *= radius;
			v += centre;

			vertices.push_back(v);
		}
	}

	/*
		The last particle is the centre particle that connects to all
		the rest.
	*/
	vertices.push_back(centre);

	return vertices;
}


std::vector<Edge*> Blob::calculateEdges() {

	std::vector<Edge*> edges;

	// Generates sphere edges based on how the vertices were
	for (int lat = 0; lat < rowSize; lat++) {
		for (int lon = 0; lon < columnSize; lon++) {
			/*
				Indices of the four vertices of the current square
				in counter clockwise order
			*/
			int v0 = lat * (columnSize + 1) + lon;
			int v1 = v0 + 1;
			int v2 = (lat + 1) * (rowSize + 1) + lon;
			int v3 = v2 + 1;

			edges.push_back(
				new Edge(&localVertices, &globalVertices, v0, v2)
			);
			edges.push_back(
				new Edge(&localVertices, &globalVertices, v3, v1)
			);
			edges.push_back(
				new Edge(&localVertices, &globalVertices, v2, v3)
			);
			edges.push_back(
				new Edge(&localVertices, &globalVertices, v1, v0)
			);
		}
	}

	for (int i = 0; i < particles.size() - 1; i++) {
		edges.push_back(
			new Edge(
				&localVertices,
				&globalVertices,
				i, particles.size() - 1
			)
		);
	}

	return edges;
}


std::vector<CurvedFace*> Blob::calculateFaces() {

	std::vector<CurvedFace*> faces;

	// Generates sphere faces based on how the vertices were
	for (int lat = 0; lat < rowSize; lat++) {
		for (int lon = 0; lon < columnSize; lon++) {
			/*
				Indices of the four vertices of the current triangle
				in counterclockwise order.
			*/
			int v[4];
			v[0] = lat * (columnSize + 1) + lon;
			v[1] = v[0] + 1;
			v[2] = (lat + 1) * (rowSize + 1) + lon;
			v[3] = v[2] + 1;

			// Forms face in counter-clockwise order
			std::vector<int> faceIndexes{ v[0], v[1], v[3], v[2] };

			/*
				Note that all the sphere faces are curved.
				The normals of the side face vertices can be calculated
				as the normal vector from the center of the sphere to
				which the vertices themselves.
				Note that since the normals are initially sent in local
				vertices, we can just consider the center to be the
				origin (0, 0, 0) and the normal of each vertex the
				vertex coordinates themselves normalized.

				The normals are sent to the face in the same order as
				the vertex indexes: counter-clockwise.
			*/

			std::vector<Vector3D> normals{
				localVertices[v[0]].normalized(),
				localVertices[v[1]].normalized(),
				localVertices[v[3]].normalized(),
				localVertices[v[2]].normalized()
			};

			CurvedFace* face = new CurvedFace(
				&localVertices,
				&globalVertices,
				faceIndexes,
				normals
			);
			faces.push_back(face);
		}
	}

	return faces;
}


void Blob::setForces() {

	for (Edge* edge : edges) {
		Vector3D distanceVector = localVertices[edge->getIndex(0)] -
			localVertices[edge->getIndex(1)];
		real distance = distanceVector.magnitude();

		// Adds the bungee force for each edge twice (one from each particle)
		SpringForce force{
			ParticleSpringDamper(
				particles[edge->getIndex(1)],
				springStrength,
				0.2,
				distance
			),
			ParticleSpringDamper(
				particles[edge->getIndex(0)],
				springStrength,
				0.2,
				distance
			),
		};
		forces.push_back(force);
	}
}


void Blob::setConstraints() {

	for (Edge* edge : edges) {
		Vector3D distanceVector = localVertices[edge->getIndex(0)] -
			localVertices[edge->getIndex(1)];
		real restLength = distanceVector.magnitude();

		// Adds the distance constraint for each edge as well
		Particle* particle1 = particles[edge->getIndex(0)];
		Particle* particle2 = particles[edge->getIndex(1)];

		ParticleDistanceConstraint distanceConstraint(
			particle1,
			particle2,
			restLength
		);
		distanceConstraints.push_back(distanceConstraint);
	}
}


void Blob::calculateMeshNormals() {

	std::vector<Vector3D> normals(particles.size());

	// First we calculate the normal of each particle
	for (CurvedFace* face : faces) {

		/*
			Since we don't have a transform matrix, we recalculate the
			values using the formulas. The normals have to be set from
			the outside, and the rest the face can calculate.
		*/
		for (int i = 0; i < face->getVertexNumber(); i++) {
			/*
				We add the face normal to the normal of each vertex in
				the face (recall that mist vertices are shared among
				several faces).
			*/
			normals[face->getIndex(i)] += face->getNormal();
		}

		face->update();
	}

	// We then normalize them all
	for (Vector3D& normal : normals) {
		normal.normalize();
	}

	// We then set the normal for each face based on the particles it contains
	for (CurvedFace* face : faces) {
		for (int i = 0; i < face->getVertexNumber(); i++) {
			face->setNormal(i, normals[face->getIndex(i)]);
		}
	}

}


void Blob::applyConstraints() {
	// Applies distance constraints
	for (int i = 0; i < distanceConstraints.size(); i++) {
		distanceConstraints[i].applyConstraint();
	}
}