
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



FaceData pe::getMeshFaceData(const ParticleMesh& mesh) {

	std::vector<glm::vec3> flattenedPositions;
	std::vector<glm::vec3> flattenedNormals;
	std::vector<glm::vec3> flattenedTangents;
	std::vector<glm::vec3> flattenedBitangents;
	std::vector<glm::vec2> flattenedUvCoordinates;

	// Flattens the polyhedron data and compute the normals
	for (CurvedFace* face : mesh.faces) {

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


EdgeData pe::getMeshEdgeData(const ParticleMesh& mesh) {

	std::vector<glm::vec3> flattenedPositions;

	// Flattens the polyhedron data and compute the normals
	for (Edge* edge : mesh.edges) {
		flattenedPositions.push_back(convertToGLM(edge->getVertex(0)));
		flattenedPositions.push_back(convertToGLM(edge->getVertex(1)));
	}
	EdgeData data{ flattenedPositions };
	return data;
}


FrameVectors pe::getMeshUniformFrameVectors(
	const ParticleMesh& mesh,
	real length
) {

	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> tangents;
	std::vector<glm::vec3> bitangents;

	for (Face* face : mesh.faces) {

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


FrameVectors pe::getMeshFrameVectors(
	const ParticleMesh& mesh,
	real length
) {

	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> tangents;
	std::vector<glm::vec3> bitangents;

	for (Face* face : mesh.faces) {

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


FaceData pe::getUniformMeshFaceData(const ParticleMesh& mesh) {
	std::vector<glm::vec3> flattenedPositions;
	std::vector<glm::vec3> flattenedNormals;

	// Flattens the polyhedron data
	for (Face* face : mesh.faces) {

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