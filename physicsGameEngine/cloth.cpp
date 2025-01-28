
#include "cloth.h"

using namespace pe;


Cloth::Cloth(
	int columnDensity, int rowDensity,
	real height, real width,
	const Vector3D& direction1,
	const Vector3D& direction2,
	const Vector3D& origin,
	real mass,
	real damping,
	real dampingCoefficient,
	real structuralStifness,
	real shearStifness,
	real bendStiffness
) : sideDensity{std::make_pair(columnDensity, rowDensity)},
	sideLength{ std::make_pair(height, width) },
	sideDirection{ std::make_pair(direction1, direction2)},
	origin{origin},
	SoftObject(std::move(
		generateSoftObject(
		columnDensity, rowDensity, height, width,
		direction1, direction2, origin,
		mass, damping, dampingCoefficient,
		structuralStifness, shearStifness, bendStiffness
	))
) {
	
	particleNeighbors.resize(body.particles.size());

	for (int n = 0; n < body.particles.size(); n++) {

		int particlesX = sideDensity.first;
		int particlesY = sideDensity.second;

		// Converting 1D index to 2D grid coordinates
		int i = n / particlesX;
		int j = n % particlesX;

		// Top
		if (i > 0) 
			particleNeighbors[n].push_back((i - 1) * particlesX + j);
		// Bottom
		if (i < particlesY - 1) 
			particleNeighbors[n].push_back((i + 1) * particlesX + j);
		// Left
		if (j > 0) 
			particleNeighbors[n].push_back(i * particlesX + (j - 1));
		// Right
		if (j < particlesX - 1) 
			particleNeighbors[n].push_back(i * particlesX + (j + 1));
	}

	// Setting the UV coordinates
	real stepX = (real)1.0 / (sideDensity.second - 1);
	real stepY = (real)1.0 / (sideDensity.first - 1);
	for (int i = 0; i < mesh.getFaceCount(); i++) {
		std::vector<Vector2D> uv(mesh.getFace(i).getVertexCount());
		for (int j = 0; j < mesh.getFace(i).getVertexCount(); j++) {
			int x = mesh.getFace(i).getIndex(j) % sideDensity.second;
			int y = 1 - (mesh.getFace(i).getIndex(j) / sideDensity.second);
			uv[j] = Vector2D(x * stepX, y * stepY);
		}
		mesh.setFaceTextureCoordinates(i, uv);
	}

}


SoftObject Cloth::generateSoftObject(
	int columnDensity, int rowDensity,
	real height, real width,
	const Vector3D& direction1,
	const Vector3D& direction2,
	const Vector3D& origin,
	real mass,
	real damping,
	real dampingCoefficient,
	real structuralStiffness,
	real shearStiffness,
	real bendStiffness
) {

	std::vector<Vector3D> particleGrid;

	// The step size in each direction
	real step1 = height / (columnDensity - 1);
	real step2 = width / (rowDensity - 1);

	for (int i = 0; i < columnDensity; ++i) {
		for (int j = 0; j < rowDensity; ++j) {
			// Particle position
			Vector3D particle = (
				origin + direction1 * (i * step1) +
				direction2 * (j * step2)
			);

			particleGrid.push_back(particle);
		}
	}

	int particlesX = rowDensity;
	int particlesY = columnDensity;

	// Generating the mesh, whose vertices are the particles themselves
	std::vector<std::vector<int>> faces;
	std::vector<std::pair<int, int>> edges;

	// We also generate the spring pairs and strengths
	std::vector<std::pair<int, int>> springPairs;
	std::vector<real> springStrengths;

	for (int i = 0; i < particlesY; i++) {
		for (int j = 0; j < particlesX; j++) {

			// Flattened index of the current particle
			int current = i * particlesX + j;

			/*
				Structural Springs(to right and below) with bidirectional
				pairs.
			*/
			if (j < particlesX - 1) {
				int right = current + 1;
				edges.push_back({ current, right });
				springPairs.push_back({ current, right });
				springPairs.push_back({ right, current });
				springStrengths.push_back(structuralStiffness);
				springStrengths.push_back(structuralStiffness);
			}
			if (i < particlesY - 1) {
				int bottom = current + particlesX;
				edges.push_back({ current, bottom });
				springPairs.push_back({ current, bottom });
				springPairs.push_back({ bottom, current });
				springStrengths.push_back(structuralStiffness);
				springStrengths.push_back(structuralStiffness);
			}

			// Shear Springs (diagonals) with bidirectional pairs
			if (i < particlesY - 1 && j < particlesX - 1) {
				int bottomRight = current + particlesX + 1;
				springPairs.push_back({ current, bottomRight });
				springPairs.push_back({ bottomRight, current });
				springStrengths.push_back(shearStiffness);
				springStrengths.push_back(shearStiffness);
			}
			if (i < particlesY - 1 && j > 0) {
				int bottomLeft = current + particlesX - 1;
				springPairs.push_back({ current, bottomLeft });
				springPairs.push_back({ bottomLeft, current });
				springStrengths.push_back(shearStiffness);
				springStrengths.push_back(shearStiffness);
			}

			/*
				Bend Springs(two steps to the right, bottom,
				diagonals) with bidirectional pairs.
			*/
			if (j < particlesX - 2) {
				int twoRight = current + 2;
				springPairs.push_back({ current, twoRight });
				springPairs.push_back({ twoRight, current });
				springStrengths.push_back(bendStiffness);
				springStrengths.push_back(bendStiffness);
			}
			if (i < particlesY - 2) { // Two steps below
				int twoDown = current + 2 * particlesX;
				springPairs.push_back({ current, twoDown });
				springPairs.push_back({ twoDown, current });
				springStrengths.push_back(bendStiffness);
				springStrengths.push_back(bendStiffness);
			}
			if (i < particlesY - 2 && j < particlesX - 2) {
				int twoDownRight = current + 2 * particlesX + 2;
				springPairs.push_back({ current, twoDownRight });
				springPairs.push_back({ twoDownRight, current });
				springStrengths.push_back(bendStiffness);
				springStrengths.push_back(bendStiffness);
			}
			if (i < particlesY - 2 && j > 1) {
				int twoDownLeft = current + 2 * particlesX - 2;
				springPairs.push_back({ current, twoDownLeft });
				springPairs.push_back({ twoDownLeft, current });
				springStrengths.push_back(bendStiffness);
				springStrengths.push_back(bendStiffness);
			}

			// The faces
			if (i < particlesY - 1 && j < particlesX - 1) {
				int bottomLeft = current + particlesX;
				int bottomRight = current + particlesX + 1;
				int topRight = current + 1;
				faces.push_back(
					{ current, topRight, bottomRight, bottomLeft }
				);
			}
		}
	}


	/*
		Then we generate the vertex - particle map, which in the case
		of cloth is 1 to 1.
	*/
	std::vector<int> vertexParticleMap(particleGrid.size());
	for (int i = 0; i < particleGrid.size(); i++) {
		vertexParticleMap[i] = i;
	}


	/*
		Finally, we define the curvature, which in the case of cloth,
		is all 4 adjacent faces, or 3 or 2 in cases of edges or corners.
	*/
	std::vector<std::vector<int>> particleFaces(particleGrid.size());
	for (int n = 0; n < particleGrid.size(); n++) {

		// 1D index n coverted to 2D grid coordinates
		int i = n / particlesX;
		int j = n % particlesX;

		// Top-left face
		if (i > 0 && j > 0) {
			particleFaces[n].push_back((i - 1)* (particlesX - 1) + (j - 1));
		}
		// Top-right face
		if (i > 0 && j < particlesX - 1) {
			particleFaces[n].push_back((i - 1)* (particlesX - 1) + j);
		}
		// Bottom-left face
		if (i < particlesY - 1 && j > 0) {
			particleFaces[n].push_back(i* (particlesX - 1) + (j - 1));
		}
		// Bottom-right face
		if (i < particlesY - 1 && j < particlesX - 1) {
			particleFaces[n].push_back(i* (particlesX - 1) + j);
		}
	}

	std::vector<std::vector<std::vector<int>>> curvatureMap(faces.size());
	for (int i = 0; i < faces.size(); i++) {
		curvatureMap[i].resize(faces[i].size());
		for (int j = 0; j < faces[i].size(); j++) {
			int particleIdx = faces[i][j];
			int faceCount = particleFaces[particleIdx].size();

			curvatureMap[i][j].resize(faceCount);
			for (int k = 0; k < faceCount; k++) {
				curvatureMap[i][j][k] = particleFaces[particleIdx][k];
			}
		}
	}

	Curvature curvature;
	curvature.curvatureMap = curvatureMap;

	Mesh mesh(particleGrid, faces, edges);

	return SoftObject(
		// The mesh
		mesh,
		// The soft body
		particleGrid, mass, damping, dampingCoefficient,
		springPairs, springStrengths,
		// Rendering data
		vertexParticleMap, curvature
	);
}


void Cloth::applyLaplacianSmoothing(int iterations, real factor) {

	for (int iteration = 0; iteration < iterations; iteration++) {

		std::vector<Vector3D> sumNeighbors(body.particles.size(), Vector3D::ZERO);

		for (int i = 0; i < body.particles.size(); i++) {
			for (int j = 0; j < particleNeighbors[i].size(); j++) {
				sumNeighbors[i] += body.particles[particleNeighbors[i][j]].position;
			}
		}

		for (int i = 0; i < body.particles.size(); ++i) {

			if (!body.particles[i].isAwake) {
				continue;
			}

			Vector3D smoothedPosition;
			// If the particle has neighbors
			if (particleNeighbors[i].size() > 0) {
				smoothedPosition = body.particles[i].position + (
					(sumNeighbors[i] / particleNeighbors[i].size()) -
					body.particles[i].position
				) * factor;
			}
			else {
				// If the particle has no neighbors, we keep its position unchanged
				smoothedPosition = body.particles[i].position;
			}

			body.particles[i].position = smoothedPosition;
		}
	}
}


void Cloth::applyWindForce(const Vector3D& force, real deltaT) {

	/*
		The wind force for each vertex in the mesh (which is likely shared
		by multiple faces).
	*/
	std::vector<ParticleDirectForce> vertexForces(mesh.getVertexCount());

	for (int i = 0; i < mesh.getFaceCount(); i++) {
		Vector3D faceNormal = mesh.getFace(i).getNormal();
		// Wind force for this face
		Vector3D windForce (faceNormal * (faceNormal.scalarProduct(force)));
		for (int j = 0; j < mesh.getFace(i).getVertexCount(); j++) {
			int vertexIndex = mesh.getFace(i).getIndex(j);
			vertexForces[vertexIndex].force += windForce;
		}
	}

	// The vertex forces are then applied to the particles that correspond to them
	for (int i = 0; i < vertexParticleMap.size(); i++) {
		vertexForces[i].updateForce(&body.particles[vertexParticleMap[i]], deltaT);
	}
}