
#include "shaderInterface_2.h"
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


FaceData triangulateFace(
	const std::vector<Vector3D>& vertices,
	const std::vector<Vector3D>& normals
) {

}


EdgeData pe::getPolyhedronEdgeData(const Polyhedron& polyhedron) {
	std::vector<glm::vec3> flattenedPositions;

	std::vector<Edge> edges = polyhedron.edges;

	for (const auto& edge : edges) {
		flattenedPositions.push_back(convertToGLM(edge.getVertex(0)));
		flattenedPositions.push_back(convertToGLM(edge.getVertex(1)));
	}
	EdgeData data{ flattenedPositions };
	return data;
}


EdgeData pe::getPolyhedronFaceNormalsData(
	const Polyhedron& polyhedron,
	real length
) {
	std::vector<glm::vec3> normals;
	for (const Face& face : polyhedron.faces) {
		normals.push_back(convertToGLM(face.getCentroid()));
		Vector3D secondPoint = face.getCentroid() 
			+ face.getNormal() * length;
		normals.push_back(convertToGLM(secondPoint));
	}

	EdgeData data{ normals };
	return data;
}


EdgeData pe::getPolyhedronVertexNormalsData(
	const Polyhedron& polyhedron,
	real length
) {
	std::vector<glm::vec3> normals;
	for (const Face& face : polyhedron.faces) {
		std::vector<Vector3D> faceNormals = face.getVertexNormals();
		for (int i = 0; i < face.getVertexNumber(); i++) {
			normals.push_back(convertToGLM(face.getVertex(i)));
			Vector3D secondPoint = face.getVertex(i)
				+ face.getNormal() * length;
			normals.push_back(convertToGLM(secondPoint));
		}
	}

	EdgeData data{ normals };
	return data;
}


FaceData pe::getPolyhedronFaceData(const Polyhedron& polyhedron) {
	std::vector<glm::vec3> flattenedPositions;
	std::vector<glm::vec3> flattenedNormals;

	// Flattens the polyhedron data and compute the normals
	for (const auto& face : polyhedron.faces) {

		std::vector<Vector3D> vertexNormals = face.getVertexNormals();

		/*
			We need to triangulate the face, while keeping the vertices of
			each triangle in the same order. Since the faces of polyhedra
			are ordered (clockwise or counter-clockwise), we can achieve
			this by anchoring the triangulation at the first vertex 0,
			then connecting it to consecutive vertices: 0-1-2, 0-2-3...
		*/
		for (int i = 1; i < face.getVertexNumber() - 1; i++) {
			int indexes[3]{ 0, i, i + 1 };
			for (int j = 0; j < 3; j++) {
				flattenedPositions.push_back(
					convertToGLM(face.getVertex(indexes[j]))
				);
				flattenedNormals.push_back(
					convertToGLM(vertexNormals[indexes[j]])
				);
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


FaceData pe::getUniformPolyhedronFaceData(const Polyhedron& polyhedron) {
	std::vector<glm::vec3> flattenedPositions;
	std::vector<glm::vec3> flattenedNormals;

	// Flattens the polyhedron data
	for (const auto& face : polyhedron.faces) {

		/*
			The normals of all vertices in the face are the same as
			the face's.
		*/
		glm::vec3 faceNormal = convertToGLM(face.getNormal());

		/*
			We need to triangulate the face, while keeping the vertices of
			each triangle in the same order. Since the faces of polyhedra
			are ordered (clockwise or counter-clockwise), we can achieve
			this by anchoring the triangulation at the first vertex 0,
			then connecting it to consecutive vertices: 0-1-2, 0-2-3...
		*/
		for (int i = 1; i < face.getVertexNumber() - 1; i++) {
			int indexes[3]{ 0, i, i + 1 };
			for (int j = 0; j < 3; j++) {
				flattenedPositions.push_back(
					convertToGLM(face.getVertex(indexes[j]))
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
