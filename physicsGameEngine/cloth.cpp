
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
	ropeStrength{ ropeStrength } {

	setFaces(calculateFaces());
	setEdges(calculateEdges());

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
				The square faces are defined by 2 columns and 2 rows,
				in counter-clockwise order.
			*/
			std::vector<int> indexes{
				i * rowSize + j,
				(i + 1) * rowSize + j,
				(i + 1) * rowSize + (j + 1),
				i * rowSize + (j + 1)
			};

			// The normals are not initially set, but will be calculated each frame
			std::vector<Vector3D>faceNormals(indexes.size(), Vector3D());

			CurvedFace* face = new CurvedFace(
				&vertices,
				indexes,
				faceNormals
			);

			std::vector<Vector2D> uv{
				Vector2D(i * (1.0 / rowSize), j * (1.0 / columnSize)),
				Vector2D((i + 1) * (1.0 / rowSize), j * (1.0 / columnSize)),
				Vector2D((i + 1) * (1.0 / rowSize), (j + 1) * (1.0 / columnSize)),
				Vector2D(i * (1.0 / rowSize), (j + 1) * (1.0 / columnSize)),
			};


			face->setTextureCoordinates(uv);
			faces.push_back(face);
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
				0.2,
				distance
			),
			ParticleSpringDamper(
				particles[edge->getIndex(0)],
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


std::vector<std::vector<Vector3D>> Cloth::calculateMeshNormals() {
	
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
	}

	// We then normalize them all
	for (Vector3D& normal : normals) {
		normal.normalize();
	}

	/*
		We then associate each particle's normal with vertices in each
		face it features in so that we can return the normals as a vector
		where each entry is a vector containing the normals of the vertices
		of one face.
	*/
	std::vector<std::vector<Vector3D>> faceNormals(faces.size());
	for (int i = 0; i < faces.size(); i++) {
		for (int j = 0; j < faces[i]->getVertexNumber(); j++) {
			faceNormals[i].push_back(normals[faces[i]->getIndex(j)]);
		}
	}

	return faceNormals;
}


void Cloth::applyConstraints() {
	// Applies distance constraints
	for (ParticleDistanceConstraint& constraint : distanceConstraints) {
		constraint.applyConstraint();
	}
}