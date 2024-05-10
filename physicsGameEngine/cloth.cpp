
#include "cloth.h"

using namespace pe;


Cloth::Cloth(
	Vector3D topLeft,
	Vector3D bottomRight,
	int rowSize,
	int columnSize,
	real mass,
	real damping,
	real ropeStrength,
	real dampingConstant
) : 
	Mesh(
		returnParticleGrid(
			columnSize,
			rowSize,
			topLeft,
			bottomRight
		),
		mass,
		damping
	),
	rowSize{ rowSize },
	columnSize{ columnSize },
	ropeStrength{ ropeStrength },
	dampingConstant{dampingConstant} {

	setFaces(calculateFaces());
	setEdges(calculateEdges());

	setForces();
	setConstraints();
}


real Cloth::calculateTriangleArea(
	const Vector3D& v0, 
	const Vector3D& v1, 
	const Vector3D& v2
) {
	real a = (v1 - v0).magnitude();
	real b = (v2 - v1).magnitude();
	real c = (v0 - v2).magnitude();
	// Semiperimeter
	real s = (a + b + c) / 2.0;
	// Heron's formula
	return realSqrt(s * (s - a) * (s - b) * (s - c));
}


std::vector<pe::Vector3D>  Cloth::returnParticleGrid(
	int columnSize,
	int rowSize,
	Vector3D topLeft,
	Vector3D bottomRight
) {
	std::vector<Vector3D> positions;

	// Step sizes
	real columnStep = (bottomRight.x - topLeft.x) /
		static_cast<real>(columnSize - 1);
	real rowStep = (bottomRight.y - topLeft.y) /
		static_cast<real>(rowSize - 1);

	for (int row = 0; row < rowSize; ++row) {
		for (int col = 0; col < columnSize; ++col) {
			real x = topLeft.x + col * columnStep;
			real y = topLeft.y + row * rowStep;
			real z = topLeft.z + (bottomRight.z - topLeft.z) *
				(col) / (columnSize - 1);

			positions.push_back(Vector3D(x, y, z));
		}
	}

	return positions;
}


std::vector<Edge*> Cloth::calculateEdges() {

	std::vector<Edge*> edges;

	/*
		Note that we only include unique edges: if we have
		an edge from particle i to j, we don't also include one
		from j to i.
	*/
	for (int i = 0; i < columnSize * rowSize; i++) {
		// Structural links (horizontal and vertical)
		if (i >= rowSize) {
			edges.push_back(
				new Edge(&vertices, i, i - rowSize)
			);
		}
		if (i % rowSize != 0) {
			edges.push_back(
				new Edge(&vertices, i, i - 1)
			);
		}
		// Diagonals (shear)
		if (i >= rowSize && i % rowSize != 0) {
			edges.push_back(
				new Edge(&vertices, i, i - rowSize - 1)
			);
		}
		if (i >= rowSize && (i + 1) % rowSize != 0) {
			edges.push_back(
				new Edge(&vertices, i, i - rowSize + 1)
			);
		}
	}

	return edges;
}


std::vector<CurvedFace*> Cloth::calculateFaces() {

	std::vector<CurvedFace*> faces;

	for (int i = 0; i < columnSize - 1; i++) {
		for (int j = 0; j < rowSize - 1; j++) {

			/*
				The faces are triangles which make up the squares
				connecting 4 vertices nect to each other in the mesh.
			*/
			std::vector<int> indexes[2]{
				{
					i * rowSize + j,
					(i + 1) * rowSize + j,
					(i + 1) * rowSize + (j + 1)
				},
				{
					(i + 1) * rowSize + (j + 1),
					i * rowSize + (j + 1),
					i * rowSize + j
				}
			};


			std::vector<Vector2D> uv[2]{
				{
					Vector2D(i * (1.0 / rowSize), j * (1.0 / columnSize)),
					Vector2D((i + 1) * (1.0 / rowSize), j * (1.0 / columnSize)),
					Vector2D((i + 1) * (1.0 / rowSize), (j + 1) * (1.0 / columnSize))
				},
				{
					Vector2D((i + 1) * (1.0 / rowSize), (j + 1) * (1.0 / columnSize)),
					Vector2D(i * (1.0 / rowSize), (j + 1) * (1.0 / columnSize)),
					Vector2D(i * (1.0 / rowSize), j * (1.0 / columnSize))
				}
			};

			for (int i = 0; i < 2; i++) {

				// The normals are not initially set, but will be calculated each frame
				std::vector<Vector3D>faceNormals(indexes[i].size(), Vector3D());

				CurvedFace* face = new CurvedFace(
					&vertices,
					indexes[i],
					faceNormals
				);

				face->setTextureCoordinates(uv[i]);
				faces.push_back(face);
			}
		}
	}

	return faces;
}


void Cloth::setForces() {

	for (Edge* edge : edges) {
		Vector3D distanceVector = vertices[edge->getIndex(0)] -
			vertices[edge->getIndex(1)];
		real distance = distanceVector.magnitude();

		// Adds the bungee force for each edge twice (one from each particle)
		SpringForce force{
			ParticleSpringDamper(
				particles[edge->getIndex(1)],
				ropeStrength,
				dampingConstant,
				distance
			),
			ParticleSpringDamper(
				particles[edge->getIndex(0)],
				ropeStrength,
				dampingConstant,
				distance
			),
		};
		forces.push_back(force);
	}
}


void Cloth::setConstraints() {

	for (Edge* edge : edges) {
		Vector3D distanceVector = vertices[edge->getIndex(0)] -
			vertices[edge->getIndex(1)];
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


void Cloth::calculateMeshNormals() {

	// Return their values to the 0 vector
	particleNormals = std::vector<Vector3D>(particles.size());

	// First we calculate the normal of each particle
	for (CurvedFace* face : faces) {

		real area = calculateTriangleArea(
			face->getVertex(0), 
			face->getVertex(1), 
			face->getVertex(2)
		);

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
			particleNormals[face->getIndex(i)] += face->getNormal() * area;
		}
	}


	// We then normalize them all
	for (Vector3D& normal : particleNormals) {
		normal.normalize();
	}
}


void Cloth::applyConstraints() {
	// Applies distance constraints
	for (ParticleDistanceConstraint& constraint : distanceConstraints) {
		constraint.applyConstraint();
	}
}


void Cloth::laplacianSmoothing(int iterations, real factor) {

	for (int iteration = 0; iteration < iterations; iteration++) {

		std::vector<Vector3D> sumNeighbors(particles.size());
		std::vector<int> countNeighbors(particles.size(), 0);

		for (Edge* edge : edges) {
			int index0 = edge->getIndex(0);
			int index1 = edge->getIndex(1);

			countNeighbors[index0]++;
			countNeighbors[index1]++;

			sumNeighbors[index0] += edge->getVertex(1);
			sumNeighbors[index1] += edge->getVertex(0);
		}

		for (int i = 0; i < particles.size(); ++i) {

			if (!particles[i]->isAwake) {
				continue;
			}

			Vector3D smoothedPosition;
			if (countNeighbors[i] > 0) {
				smoothedPosition = particles[i]->position + (
					(sumNeighbors[i] / countNeighbors[i]) - particles[i]->position
				) * factor;
			}
			else {
				// If the particle has no neighbors, we keep its position unchanged
				smoothedPosition = particles[i]->position;
			}

			particles[i]->position = smoothedPosition;
		}
	}
}
