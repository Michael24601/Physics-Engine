
#include "cloth.h"

using namespace pe;


Cloth::Cloth(
	Vector3D topLeft,
	Vector3D bottomRight,
	int rowSize,
	int columnSize,
	real mass,
	real damping,
	real ropeStrength
) : rowSize{ rowSize },
columnSize{ columnSize },
ropeStrength{ ropeStrength } {

	std::vector<Vector3D> particlePositions = returnParticleGrid(
		columnSize,
		rowSize,
		topLeft,
		bottomRight
	);

	localVertices = particlePositions;
	/*
		Initially, the global vertices were the same as the
		original local ones.
	*/
	globalVertices = particlePositions;

	/*
		Positions(in global coordinates, as we are working with
		particles, and there are no transformations.
	*/
	particles.resize(particlePositions.size());
	for (int i = 0; i < particles.size(); i++) {
		particles[i].position = particlePositions[i];
		particles[i].setMass(mass);
		particles[i].damping = damping;
	}


	setEdges();
	setFaces();
	setForces();
	setConstraints();
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


void Cloth::setEdges() {
	/*
		Note that we only include unique edges: if we have
		an edge from particle i to j, we don't also include one
		from j to i.
	*/
	for (int i = 0; i < columnSize * rowSize; i++) {
		// Structural links (horizontal and vertical)
		if (i >= rowSize) {
			edges.push_back(
				new Edge(&localVertices, &globalVertices, i, i - rowSize)
			);
		}
		if (i % rowSize != 0) {
			edges.push_back(
				new Edge(&localVertices, &globalVertices, i, i - 1)
			);
		}
		// Diagonals (shear)
		if (i >= rowSize && i % rowSize != 0) {
			edges.push_back(
				new Edge(&localVertices, &globalVertices, i, i - rowSize - 1)
			);
		}
		if (i >= rowSize && (i + 1) % rowSize != 0) {
			edges.push_back(
				new Edge(&localVertices, &globalVertices, i, i - rowSize + 1)
			);
		}
	}
}


void Cloth::setFaces() {

	for (int i = 0; i < columnSize - 1; i++) {
		for (int j = 0; j < rowSize - 1; j++) {

			/*
				Triangle faces, which divide each square into 2
				(The squares are defined by 2 columns and 2 rows,
				in counter-clockwise order).
				Here are the indexes of the two triangles of
				the current square.
				We use triangles instead of square faces as
				they will make normal calculation easier later.
			*/
			std::vector<std::vector<int>> faceIndexes{
				std::vector<int>{
					i* rowSize + j,
					(i + 1) * rowSize + j,
					(i + 1) * rowSize + (j + 1)
				},
				std::vector<int>{
					(i + 1) * rowSize + (j + 1),
					i * rowSize + (j + 1),
					i * rowSize + j
				}
			};

			// The uv coordinates of both triangles, which should form a square
			std::vector<std::vector<Vector2D>> uv{
				std::vector<Vector2D>{
					Vector2D(i* (1.0 / rowSize), j* (1.0 / columnSize)),
					Vector2D((i + 1)* (1.0 / rowSize), j* (1.0 / columnSize)),
					Vector2D((i + 1)* (1.0 / rowSize), (j + 1)* (1.0 / columnSize))
				},
				std::vector<Vector2D>{
					Vector2D((i + 1)* (1.0 / rowSize), (j + 1)* (1.0 / columnSize)),
					Vector2D(i* (1.0 / rowSize), (j + 1)* (1.0 / columnSize)),
					Vector2D(i * (1.0 / rowSize), j * (1.0 / columnSize))
				}
			};

			// We set each of the two faces here
			for (int i = 0; i < 2; i++) {

				// The normals are initially just the 0 vector
				std::vector<Vector3D>faceNormals(faceIndexes[i].size(), Vector3D());

				CurvedFace* face = new CurvedFace(
					&localVertices,
					&globalVertices,
					faceIndexes[i],
					faceNormals
				);

				face->setTextureCoordinates(uv[i]);

				faces.push_back(face);
			}

		}
	}
}


void Cloth::setForces() {

	for (Edge* edge : edges) {
		Vector3D distanceVector = localVertices[edge->getIndex(0)] -
			localVertices[edge->getIndex(1)];
		real distance = distanceVector.magnitude();

		// Adds the bungee force for each edge twice (one from each particle)
		SpringForce force{
			ParticleSpringDamper(
				&particles[edge->getIndex(1)],
				ropeStrength,
				0.2,
				distance
			),
			ParticleSpringDamper(
				&particles[edge->getIndex(0)],
				ropeStrength,
				0.2,
				distance
			),
		};
		forces.push_back(force);
	}
}


void Cloth::setConstraints() {

	for (Edge* edge : edges) {
		Vector3D distanceVector = localVertices[edge->getIndex(0)] -
			localVertices[edge->getIndex(1)];
		real restLength = distanceVector.magnitude();

		// Adds the distance constraint for each edge as well
		Particle* particle1 = &particles[edge->getIndex(0)];
		Particle* particle2 = &particles[edge->getIndex(1)];

		ParticleDistanceConstraint distanceConstraint(
			particle1,
			particle2,
			restLength
		);
		distanceConstraints.push_back(distanceConstraint);
	}
}


void Cloth::calculateMeshNormals() {
	
	std::vector<Vector3D> normals(particles.size());

	// First we calculate the normal of each particle
	for (CurvedFace* face : faces) {

		Vector3D side1 = face->getVertex(1) - face->getVertex(0);
		Vector3D side2 = face->getVertex(2) - face->getVertex(0);
		Vector3D normal = (side1.vectorProduct(side2)).normalized();

		/*
			Since we don't have a transform matrix, we recalculate the
			values using the formulas. The normals have to be set from
			the outside, and the rest the face can calculate.
		*/
		for (int i = 0; i < face->getVertexNumber(); i++) {
			normals[face->getIndex(i)] += normal;
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


void Cloth::applyConstraints() {
	// Apply distance constraints
	for (ParticleDistanceConstraint& constraint : distanceConstraints) {
		constraint.applyConstraint();
	}

	// Apply angle constraints
	for (ParticleAngleConstraint& constraint : angleConstraints) {
		constraint.applyConstraint();
	}
}


void Cloth::update() {

	/*
		First the global vertices need to be updated to be the same
		as the particle's as there is not transform matrix.
	*/
	for (int i = 0; i < particles.size(); i++) {
		globalVertices[i] = particles[i].position;
	}

	calculateMeshNormals();
}