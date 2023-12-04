
#include "polyhedronInterface.h"

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