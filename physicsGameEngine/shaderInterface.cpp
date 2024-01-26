
#include "shaderInterface.h"

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


std::vector<std::vector<Vector3D>> pe::triangulateFace(
	const std::vector<Vector3D>& vertices
) {
	std::vector<std::vector<Vector3D>> triangles;

	if (vertices.size() < 3) {
		// Not enough vertices to form a polygon
		return triangles;
	}

	for (size_t i = 1; i < vertices.size() - 1; ++i) {
		triangles.push_back({
			vertices[0],
			vertices[i],
			vertices[i + 1]
			});
	}

	return triangles;
}


edgeData pe::getPolyhedronEdgeData(const Polyhedron& polyhedron) {
	std::vector<glm::vec3> flattenedPositions;

	std::vector<Edge> edges = polyhedron.edges;

	for (const auto& edge : edges) {
		flattenedPositions.push_back(convertToGLM(edge.vertices.first));
		flattenedPositions.push_back(convertToGLM(edge.vertices.second));
	}
	edgeData data{ flattenedPositions };
	return data;
}


edgeData pe::getPolyhedronNormalsData(
	const Polyhedron& polyhedron,
	real length
) {
	std::vector<glm::vec3> normals;
	for (const Face& face : polyhedron.faces) {
		normals.push_back(convertToGLM(face.centroid));
		Vector3D secondPoint = face.centroid + face.normal * length;
		normals.push_back(convertToGLM(secondPoint));
	}

	edgeData data{ normals };
	return data;
}


faceData pe::getPolyhedronFaceData(const Polyhedron& polyhedron) {
	std::vector<glm::vec3> flattenedPositions;
	std::vector<glm::vec3> flattenedNormals;

	std::vector<Face> faces = polyhedron.faces;

	// Flatten the cube data and triangulate the faces
	for (const auto& face : faces) {

		auto triangles = triangulateFace(face.vertices);
		flattenedPositions.reserve(triangles.size() * 3);
		flattenedNormals.reserve(triangles.size() * 3);

		for (const auto& triangle : triangles) {
			flattenedPositions.push_back(convertToGLM(triangle[0]));
			flattenedPositions.push_back(convertToGLM(triangle[1]));
			flattenedPositions.push_back(convertToGLM(triangle[2]));

			/*
				The normal of each vertex in the triangle is the same
				as the normal of the face from which the triangle is
				extracted (because this is a polyhedron, with no
				curved surfaces.
			*/
			glm::vec3 faceNormal = convertToGLM(face.normal);
			flattenedNormals.push_back(faceNormal);
			flattenedNormals.push_back(faceNormal);
			flattenedNormals.push_back(faceNormal);
		}
	}

	faceData data{ flattenedPositions, flattenedNormals };
	return data;
}


faceData pe::getSphereFaceData(const SolidSphere& sphere) {
	std::vector<glm::vec3> flattenedPositions;
	std::vector<glm::vec3> flattenedNormals;

	std::vector<Face> faces = sphere.faces;

	// Flatten the cube data and triangulate the faces
	for (const auto& face : faces) {
		auto triangles = triangulateFace(face.vertices);
		flattenedPositions.reserve(triangles.size() * 3);
		flattenedNormals.reserve(triangles.size() * 3);

		for (const auto& triangle : triangles) {

			for (const auto& vertex : triangle) {
				flattenedPositions.push_back(convertToGLM(vertex));
				// The center is the body's position
				Vector3D normal = vertex - sphere.body->position;
				normal.normalize();
				flattenedNormals.push_back(convertToGLM(normal));
			}
		}
	}

	faceData data{ flattenedPositions, flattenedNormals };
	return data;
}


faceData pe::getCylinderFaceData(const Cylinder& cylinder) {
	std::vector<glm::vec3> flattenedPositions;
	std::vector<glm::vec3> flattenedNormals;

	std::vector<Face> faces = cylinder.faces;
	
	// First, the top and bottom faces

	// Flatten the cube data and triangulate the faces
	for (int i = 0; i < 2; i++){
		Face face = faces[i];

		auto triangles = triangulateFace(face.vertices);
		flattenedPositions.reserve(triangles.size() * 3);
		flattenedNormals.reserve(triangles.size() * 3);

		for (const auto& triangle : triangles) {
			flattenedPositions.push_back(convertToGLM(triangle[0]));
			flattenedPositions.push_back(convertToGLM(triangle[1]));
			flattenedPositions.push_back(convertToGLM(triangle[2]));

			glm::vec3 faceNormal = convertToGLM(face.normal);
			flattenedNormals.push_back(faceNormal);
			flattenedNormals.push_back(faceNormal);
			flattenedNormals.push_back(faceNormal);
		}
	}

	// And for the strips along the curved surface

	for (size_t i = 2; i < faces.size(); ++i) {
		Face face = faces[i];

		auto triangles = triangulateFace(face.vertices);
		flattenedPositions.reserve(triangles.size() * 3);
		flattenedNormals.reserve(triangles.size() * 3);

		for (const auto& triangle : triangles) {
			for (const auto& vertex : triangle) {
				flattenedPositions.push_back(convertToGLM(vertex));
				
				/*
					First we check to see if the vertex is on the top or
					bottom face.
					The correct normal is the one that comes from the closer
					center, so we claculate both and use the one with the
					smaller magnitude. This is because the vertices are
					all equidistant from the centroids of the faces they
					belong to, and that distance is always smaller than the
					other centroid, which we are sure of because it is
					parallel to the first face.
				*/
				Vector3D normalFromFace0 = vertex - faces[0].centroid;
				Vector3D normalFromFace1 = vertex - faces[1].centroid;
				if (normalFromFace0.magnitude() < normalFromFace1.magnitude()) {
					normalFromFace0.normalize();
					flattenedNormals.push_back(convertToGLM(normalFromFace0));
				}
				else {
					normalFromFace1.normalize();
					flattenedNormals.push_back(convertToGLM(normalFromFace1));
				}
			}
		}
	}

	faceData data{ flattenedPositions, flattenedNormals };
	return data;
}


faceData pe::getConeFaceData(const Cone& cone) {
	std::vector<glm::vec3> flattenedPositions;
	std::vector<glm::vec3> flattenedNormals;

	Vector3D apex = cone.globalVertices[0];

	std::vector<Face> faces = cone.faces;

	/*
		The first face is the base and all the vertices have
		the same normal as it is flat.
	*/
	Face base = faces[0];

	// The center of the base
	Vector3D baseCenter = (cone.body->position - apex) 
		+ cone.body->position;

	/*
		The normal of the bottom circular face is the opposite of the
		center of the cone to apex
	*/
	Vector3D bottomNormal = cone.body->position - apex;
	bottomNormal.normalize();

	auto triangles = triangulateFace(base.vertices);
	for (const auto& triangle : triangles) {
		for (const auto& vertex : triangle) {
			flattenedPositions.push_back(convertToGLM(vertex));
			flattenedNormals.push_back(convertToGLM(bottomNormal));
		}
	}

	// Curved triangle strips
	for (size_t i = 1; i < faces.size(); ++i) {
		Face face = faces[i];

		flattenedPositions.push_back(convertToGLM(face.vertices[0]));
		flattenedPositions.push_back(convertToGLM(face.vertices[1]));
		flattenedPositions.push_back(convertToGLM(face.vertices[2]));

		Vector3D normalBase1 = face.vertices[1] - base.centroid;
		Vector3D normalBase2 = face.vertices[2] - base.centroid;
		Vector3D normalApex = normalBase1 + normalBase2;

		normalApex.normalize();
		normalBase1.normalize();
		normalBase2.normalize();

		flattenedNormals.push_back(convertToGLM(normalApex));
		flattenedNormals.push_back(convertToGLM(normalBase1));
		flattenedNormals.push_back(convertToGLM(normalBase2));
	}

	faceData data{ flattenedPositions, flattenedNormals };
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


faceData pe::getSmoothMeshFaceData(
	const ClothWithBungeeCord& mesh,
	int columnSize,
	int rowSize,
	order order
) {

	std::vector<glm::vec3> particleNormals;
	for (int i = 0; i < mesh.particles.size(); i++) {
		glm::vec3 normal = calculateMeshVertexNormal(
			mesh.particles,
			i,
			columnSize,
			rowSize
		);
		if (order == order::COUNTER_CLOCKWISE) {
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
			if (order == order::COUNTER_CLOCKWISE) {
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
			else if (order == order::CLOCKWISE) {
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

	return faceData{ flattenedPositions, flattenedNormals };
}


faceData pe::getMeshFaceData(
	const ParticleMesh& mesh,
	int columnSize,
	int rowSize,
	order order
) {
	std::vector<glm::vec3> flattenedPositions;
	std:: vector<glm::vec3> flattenedNormals;

	// Flattens the data and triangulates the faces
	for (const auto& face : mesh.faces) {

		auto triangles = triangulateFace(face.getVertices(order));
		flattenedPositions.reserve(triangles.size() * 3);
		flattenedNormals.reserve(triangles.size() * 3);

		for (const auto& triangle : triangles) {
			flattenedPositions.push_back(convertToGLM(triangle[0]));
			flattenedPositions.push_back(convertToGLM(triangle[1]));
			flattenedPositions.push_back(convertToGLM(triangle[2]));

			/*
				The normal of each vertex in the triangle is the same
				as the normal of the face from which the triangle is
				extracted (because this is a polyhedron, with no
				curved surfaces.
			*/
			glm::vec3 faceNormal = convertToGLM(face.getNormal(order));
			flattenedNormals.push_back(faceNormal);
			flattenedNormals.push_back(faceNormal);
			flattenedNormals.push_back(faceNormal);
		}
	}

	return faceData{ flattenedPositions, flattenedNormals};
}


edgeData pe::getMeshEdgeData(
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
	return edgeData{ flattenedPositions };
}