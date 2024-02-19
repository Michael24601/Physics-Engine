
#include "shaderInterface.h"
#include "cloth.h"

using namespace pe;


glm::mat4 pe::convertToGLM(const Matrix3x4& m) {
	return glm::mat4(
		m.data[0], m.data[4], m.data[8], 0.0f,
		m.data[1], m.data[5], m.data[9], 0.0f,
		m.data[2], m.data[6], m.data[10], 0.0f,
		m.data[3], m.data[7], m.data[11], 1.0f
	);
}


glm::vec3 pe::convertToGLM(const Vector3D& v) {
	return glm::vec3(v.x, v.y, v.z);
}


glm::vec2 pe::convertToGLM(const Vector2D& v) {
	return glm::vec2(v.x, v.y);
}


EdgeData pe::getPolyhedronEdgeData(const Polyhedron& polyhedron) {
	std::vector<glm::vec3> flattenedPositions;

	for (Edge* edge : polyhedron.edges) {
		flattenedPositions.push_back(convertToGLM(edge->getVertex(0)));
		flattenedPositions.push_back(convertToGLM(edge->getVertex(1)));
	}
	EdgeData data{ flattenedPositions };
	return data;
}


FrameVectors pe::getPolyhedronUniformFrameVectors(
	const Polyhedron& polyhedron,
	real length
) {

	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> tangents;
	std::vector<glm::vec3> bitangents;

	for (Face* face : polyhedron.faces) {

		normals.push_back(convertToGLM(face->getCentroid()));
		Vector3D secondPoint = (face->getCentroid()
			+ face->getNormal() * length);
		normals.push_back(convertToGLM(secondPoint));

		tangents.push_back(convertToGLM(face->getCentroid()));
		secondPoint = (face->getCentroid()
			+ face->getTangent() * length);
		tangents.push_back(convertToGLM(secondPoint));

		bitangents.push_back(convertToGLM(face->getCentroid()));
		secondPoint = (face->getCentroid()
			+ face->getBitangent() * length);
		bitangents.push_back(convertToGLM(secondPoint));

	}

	FrameVectors data{ normals, tangents, bitangents };
	return data;
}


FrameVectors pe::getPolyhedronFrameVectors(
	const Polyhedron& polyhedron,
	real length
) {

	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> tangents;
	std::vector<glm::vec3> bitangents;

	for (Face* face : polyhedron.faces) {

		for (int i = 0; i < face->getVertexNumber(); i++) {

			normals.push_back(convertToGLM(face->getVertex(i)));
			Vector3D secondPoint = face->getVertex(i)
				+ face->getVertexNormal(i) * length;
			normals.push_back(convertToGLM(secondPoint));

			tangents.push_back(convertToGLM(face->getVertex(i)));
			secondPoint = face->getVertex(i)
				+ face->getVertexTangent(i) * length;
			tangents.push_back(convertToGLM(secondPoint));

			bitangents.push_back(convertToGLM(face->getVertex(i)));
			secondPoint = face->getVertex(i)
				+ face->getVertexBitangent(i) * length;
			bitangents.push_back(convertToGLM(secondPoint));

		}
	}

	FrameVectors data{ normals, tangents, bitangents };
	return data;
}


FaceData pe::getPolyhedronFaceData(const Polyhedron& polyhedron) {

	std::vector<glm::vec3> flattenedPositions;
	std::vector<glm::vec3> flattenedNormals;
	std::vector<glm::vec3> flattenedTangents;
	std::vector<glm::vec3> flattenedBitangents;
	std::vector<glm::vec2> flattenedUvCoordinates;

	// Flattens the polyhedron data and compute the normals
	for (Face* face : polyhedron.faces) {

		/*
			We need to triangulate the face, while keeping the vertices of
			each triangle in the same order. Since the faces of polyhedra
			are ordered (clockwise or counter-clockwise), we can achieve
			this by anchoring the triangulation at the first vertex 0,
			then connecting it to consecutive vertices: 0-1-2, 0-2-3...
		*/
		for (int i = 1; i < face->getVertexNumber() - 1; i++) {
			int indexes[3]{ 0, i, i + 1 };
			for (int j = 0; j < 3; j++) {
				flattenedPositions.push_back(
					convertToGLM(face->getVertex(indexes[j]))
				);
				flattenedNormals.push_back(
					convertToGLM(face->getVertexNormal(indexes[j]))
				);

				flattenedTangents.push_back(
					convertToGLM(face->getVertexTangent(indexes[j]))
				);
				flattenedBitangents.push_back(
					convertToGLM(face->getVertexBitangent(indexes[j]))
				);

				flattenedUvCoordinates.push_back(
					convertToGLM(face->getTextureCoordinate(indexes[j]))
				);
			}
		}
	}

	// Constructs the FaceData struct and return
	FaceData data{
		flattenedPositions,
		flattenedNormals,
		flattenedTangents,
		flattenedBitangents,
		flattenedUvCoordinates
	};
	return data;
}


FaceData pe::getUniformPolyhedronFaceData(const Polyhedron& polyhedron) {
	std::vector<glm::vec3> flattenedPositions;
	std::vector<glm::vec3> flattenedNormals;

	// Flattens the polyhedron data
	for (Face* face : polyhedron.faces) {

		/*
			The normals of all vertices in the face are the same as
			the face's.
		*/
		glm::vec3 faceNormal = convertToGLM(face->getNormal());

		/*
			We need to triangulate the face, while keeping the vertices of
			each triangle in the same order. Since the faces of polyhedra
			are ordered (clockwise or counter-clockwise), we can achieve
			this by anchoring the triangulation at the first vertex 0,
			then connecting it to consecutive vertices: 0-1-2, 0-2-3...
		*/
		for (int i = 1; i < face->getVertexNumber() - 1; i++) {
			int indexes[3]{ 0, i, i + 1 };
			for (int j = 0; j < 3; j++) {
				flattenedPositions.push_back(
					convertToGLM(face->getVertex(indexes[j]))
				);
				flattenedNormals.push_back(faceNormal);
			}
		}
	}

	// Construct the FaceData struct and return
	FaceData data{
		flattenedPositions,
		flattenedNormals
	};
	return data;
}



glm::vec3 pe::calculateMeshVertexNormal(
	const std::vector<Particle>& particles,
	int targetIndex,
	int columnSize,
	int rowSize
) {
	glm::vec3 averageNormal(0.0f);

	// Find the index of the target particle in the array
	Particle targetParticle = particles[targetIndex];
	glm::vec3 targetPosition = convertToGLM(targetParticle.position);

	// Calculate indices of adjacent particles (assuming a regular grid)
	int row = targetIndex / rowSize;
	int col = targetIndex % rowSize;

	int leftIndex = (col > 0) ? targetIndex - 1 : -1;
	int rightIndex = (col < rowSize - 1) ? targetIndex + 1 : -1;
	int topIndex = (row > 0) ? targetIndex - rowSize : -1;
	int bottomIndex = (row < rowSize - 1) ? targetIndex + rowSize : -1;

	// Checks if adjacent particles are within bounds
	if (leftIndex != -1) {
		glm::vec3 leftPosition = convertToGLM(particles[leftIndex].position);
		averageNormal += glm::cross(leftPosition -
			targetPosition, glm::vec3(0, 1, 0));
	}
	if (rightIndex != -1) {
		glm::vec3 rightPosition = convertToGLM(particles[rightIndex].position);
		averageNormal += glm::cross(glm::vec3(0, 1, 0),
			rightPosition - targetPosition);
	}
	if (topIndex != -1) {
		glm::vec3 topPosition = convertToGLM(particles[topIndex].position);
		averageNormal += glm::cross(topPosition -
			targetPosition, glm::vec3(1, 0, 0));
	}
	if (bottomIndex != -1) {
		glm::vec3 bottomPosition = convertToGLM(particles[bottomIndex].position);
		averageNormal += glm::cross(glm::vec3(1, 0, 0),
			bottomPosition - targetPosition);
	}

	// Normalizes the average normal
	averageNormal = glm::normalize(averageNormal);

	return averageNormal;
}


FaceData pe::getSmoothMeshFaceData(
	const Cloth& mesh,
	int columnSize,
	int rowSize,
	Order order
) {

	std::vector<glm::vec3> particleNormals;
	for (int i = 0; i < mesh.particles.size(); i++) {
		glm::vec3 normal = calculateMeshVertexNormal(
			mesh.particles,
			i,
			columnSize,
			rowSize
		);
		if (order == Order::COUNTER_CLOCKWISE) {
			normal *= -1;
		}
		particleNormals.push_back(normal);
	}

	std::vector<glm::vec3> flattenedPositions;
	std::vector<glm::vec3> flattenedNormals;

	for (int i = 0; i < columnSize - 1; ++i) {
		for (int j = 0; j < rowSize - 1; ++j) {

			int targetIndex = i * rowSize + j;

			// Indices
			int topLeft = i * rowSize + j;
			int topRight = i * rowSize + (j + 1);
			int bottomLeft = (i + 1) * rowSize + j;
			int bottomRight = (i + 1) * rowSize + (j + 1);

			// We add two triangles for the square
			// Note that for this shader, we must triangulate by hand
			if (order == Order::COUNTER_CLOCKWISE) {
				flattenedPositions.push_back(convertToGLM(mesh.particles[topRight].position));
				flattenedPositions.push_back(convertToGLM(mesh.particles[topLeft].position));
				flattenedPositions.push_back(convertToGLM(mesh.particles[bottomLeft].position));

				flattenedPositions.push_back(convertToGLM(mesh.particles[topRight].position));
				flattenedPositions.push_back(convertToGLM(mesh.particles[bottomLeft].position));
				flattenedPositions.push_back(convertToGLM(mesh.particles[bottomRight].position));

				// Add normals for each vertex
				flattenedNormals.push_back(particleNormals[topRight]);
				flattenedNormals.push_back(particleNormals[topLeft]);
				flattenedNormals.push_back(particleNormals[bottomLeft]);

				flattenedNormals.push_back(particleNormals[topRight]);
				flattenedNormals.push_back(particleNormals[bottomLeft]);
				flattenedNormals.push_back(particleNormals[bottomRight]);
			}
			else if (order == Order::CLOCKWISE) {
				flattenedPositions.push_back(convertToGLM(mesh.particles[topLeft].position));
				flattenedPositions.push_back(convertToGLM(mesh.particles[topRight].position));
				flattenedPositions.push_back(convertToGLM(mesh.particles[bottomLeft].position));

				flattenedPositions.push_back(convertToGLM(mesh.particles[bottomLeft].position));
				flattenedPositions.push_back(convertToGLM(mesh.particles[topRight].position));
				flattenedPositions.push_back(convertToGLM(mesh.particles[bottomRight].position));

				// Add normals for each vertex
				flattenedNormals.push_back(particleNormals[topLeft]);
				flattenedNormals.push_back(particleNormals[topRight]);
				flattenedNormals.push_back(particleNormals[bottomLeft]);

				flattenedNormals.push_back(particleNormals[bottomLeft]);
				flattenedNormals.push_back(particleNormals[topRight]);
				flattenedNormals.push_back(particleNormals[bottomRight]);
			}
		}
	}

	return FaceData{ flattenedPositions, flattenedNormals };
}


FaceData pe::getMeshFaceData(
	const ParticleMesh& mesh,
	int columnSize,
	int rowSize,
	Order order
) {
	std::vector<glm::vec3> flattenedPositions;
	std::vector<glm::vec3> flattenedNormals;

	// Flattens the data and triangulates the faces
	for (const auto& face : mesh.faces) {

		/*
			We need to triangulate the face, while keeping the vertices of
			each triangle in the same order. Since the faces of polyhedra
			are ordered (clockwise or counter-clockwise), we can achieve
			this by anchoring the triangulation at the first vertex 0,
			then connecting it to consecutive vertices: 0-1-2, 0-2-3...
		*/
		for (int i = 1; i < face.getParticleNumber() - 1; i++) {

			int indexes[3]{ 0, i, i + 1 };
			glm::vec3 faceNormal = convertToGLM(face.getNormal(order));

			for (int j = 0; j < 3; j++) {
				flattenedPositions.push_back(
					convertToGLM(face.getParticle(i).position)
				);
				flattenedNormals.push_back(
					faceNormal
				);
			}
		}
	}

	return FaceData{ flattenedPositions, flattenedNormals };
}


EdgeData pe::getMeshEdgeData(
	const ParticleMesh& mesh
) {
	std::vector<glm::vec3> flattenedPositions;
	for (const MeshEdge& edge : mesh.edges) {
		flattenedPositions.push_back(
			convertToGLM(edge.particles.first->position)
		);
		flattenedPositions.push_back(
			convertToGLM(edge.particles.second->position)
		);
	}
	return EdgeData{ flattenedPositions };
}