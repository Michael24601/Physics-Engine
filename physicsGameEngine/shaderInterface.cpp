
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



void pe::getEdgeData(Edge* edge, EdgeData* data) {
	data->vertices.push_back(convertToGLM(edge->getVertex(0)));
	data->vertices.push_back(convertToGLM(edge->getVertex(1)));
}


void pe::getUniformFrameVectors(
	Face* face,
	real length,
	FrameVectors* data
) {

	data->normals.push_back(convertToGLM(face->getCentroid()));
	Vector3D secondPoint = (face->getCentroid()
		+ face->getNormal() * length);
	data->normals.push_back(convertToGLM(secondPoint));

	data->tangents.push_back(convertToGLM(face->getCentroid()));
	secondPoint = (face->getCentroid()
		+ face->getTangent() * length);
	data->tangents.push_back(convertToGLM(secondPoint));

	data->bitangents.push_back(convertToGLM(face->getCentroid()));
	secondPoint = (face->getCentroid()
		+ face->getBitangent() * length);
	data->bitangents.push_back(convertToGLM(secondPoint));
}


void pe::getFrameVectors(
	Face* face,
	real length,
	FrameVectors* data
) {

	for (int i = 0; i < face->getVertexNumber(); i++) {

		data->normals.push_back(convertToGLM(face->getVertex(i)));
		Vector3D secondPoint = face->getVertex(i)
			+ face->getVertexNormal(i) * length;
		data->normals.push_back(convertToGLM(secondPoint));

		data->tangents.push_back(convertToGLM(face->getVertex(i)));
		secondPoint = face->getVertex(i)
			+ face->getVertexTangent(i) * length;
		data->tangents.push_back(convertToGLM(secondPoint));

		data->bitangents.push_back(convertToGLM(face->getVertex(i)));
		secondPoint = face->getVertex(i)
			+ face->getVertexBitangent(i) * length;
		data->bitangents.push_back(convertToGLM(secondPoint));
	}
}


void pe::getFaceData(Face* face, FaceData* data) {

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
			data->vertices.push_back(
				convertToGLM(face->getVertex(indexes[j]))
			);
			data->normals.push_back(
				convertToGLM(face->getVertexNormal(indexes[j]))
			);

			data->tangents.push_back(
				convertToGLM(face->getVertexTangent(indexes[j]))
			);
			data->bitangents.push_back(
				convertToGLM(face->getVertexBitangent(indexes[j]))
			);

			data->uvCoordinates.push_back(
				glm::vec3(convertToGLM(face->getTextureCoordinate(indexes[j])), 0.0)
			);
		}
	}
}


void pe::getUniformFaceData(Face* face, FaceData* data) {

	/*
		The normals of all vertices in the face are the same as
		the face's.
		Saeme for tangents, biTangents... etc. So we can convert
		them here and save some time.
	*/
	glm::vec3 faceNormal = convertToGLM(face->getNormal());
	glm::vec3 faceTangent = convertToGLM(face->getTangent());
	glm::vec3 faceBitangent = convertToGLM(face->getBitangent());

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
			data->vertices.push_back(
				convertToGLM(face->getVertex(indexes[j]))
			);
			data->normals.push_back(faceNormal);
			data->tangents.push_back(faceTangent);
			data->bitangents.push_back(faceBitangent);
			data->uvCoordinates.push_back(
				glm::vec3(convertToGLM(face->getTextureCoordinate(indexes[j])), 0.0)
			);
		}
	}
}


EdgeData pe::getEdgeData(const Polyhedron& polyhedron) {
	EdgeData data;
	for (Edge* edge: polyhedron.edges) {
		getEdgeData(edge, &data);
	}
	return data;
}

FrameVectors pe::getUniformFrameVectors(
	const Polyhedron& polyhedron,
	real length
) {
	FrameVectors data;
	for (Face* face : polyhedron.faces) {
		getUniformFrameVectors(face, length, &data);
	}
	return data;
}

FrameVectors pe::getFrameVectors(
	const Polyhedron& polyhedron,
	real length
) {
	FrameVectors data;
	for (Face* face : polyhedron.faces) {
		getFrameVectors(face, length, &data);
	}
	return data;
}

FaceData pe::getFaceData(const Polyhedron& polyhedron) {
	FaceData data;
	for (Face* face : polyhedron.faces) {
		getFaceData(face, &data);
	}
	return data;
}

FaceData pe::getUniformFaceData(const Polyhedron& polyhedron) {
	FaceData data;
	for (Face* face : polyhedron.faces) {
		getUniformFaceData(face, &data);
	}
	return data;
}


EdgeData pe::getEdgeData(const Mesh& mesh) {
	EdgeData data;
	for (Edge* edge : mesh.edges) {
		getEdgeData(edge, &data);
	}
	return data;
}

FrameVectors pe::getUniformFrameVectors(
	const Mesh& mesh,
	real length
) {
	FrameVectors data;
	for (Face* face : mesh.faces) {
		getUniformFrameVectors(face, length, &data);
	}
	return data;
}

FrameVectors pe::getFrameVectors(
	const Mesh& mesh,
	real length
) {
	FrameVectors data;
	for (Face* face : mesh.faces) {
		getFrameVectors(face, length, &data);
	}
	return data;
}

EdgeData pe::getCollisionBoxData(const Polyhedron& polyhedron) {

	EdgeData data;
	Vector3D halfsize = polyhedron.getHalfsize();
	Vector3D vertices[8]{
		halfsize.componentProduct(Vector3D(-1,-1, -1)),
		halfsize.componentProduct(Vector3D(1, -1, -1)),
		halfsize.componentProduct(Vector3D(1, -1, 1)),
		halfsize.componentProduct(Vector3D(-1, -1, 1)),
		halfsize.componentProduct(Vector3D(-1, 1, -1)),
		halfsize.componentProduct(Vector3D(1, 1, -1)),
		halfsize.componentProduct(Vector3D(1, 1, 1)),
		halfsize.componentProduct(Vector3D(-1, 1, 1)),
	};

	Box box(polyhedron);

	for (Vector3D& vector : vertices) {
		/*
			As mentioned earlier, the smallest box that contains a polyhedron
			may not be centred (the halfsize may not come out) of the
			centre of gravity of the polyhedron, as it may not be the
			geometric centre of the polyhedron.
			As such, we can't use the transform matrix of the polyhedron,
			but of the box, which is the same as the polyhedron but adds
			the offset to the translation.
		*/
		vector = box.transformMatrix.transform(vector);
	}

	const std::vector<std::pair<int, int>> edges{
		{0, 1}, {1, 2}, {2, 3}, {3, 0}, // Front face
		{4, 5}, {5, 6}, {6, 7}, {7, 4}, // Back face
		{0, 4}, {1, 5}, {2, 6}, {3, 7}  // Edges between front and back faces
	};

	// Populate edge data with actual vertices
	for (const auto& edge : edges) {
		Vector3D start = vertices[edge.first];
		Vector3D end = vertices[edge.second];
		data.vertices.push_back(convertToGLM(start));
		data.vertices.push_back(convertToGLM(end));
	}

	return data;
}

FaceData pe::getFaceData(const Mesh& mesh) {
	FaceData data;
	for (Face* face : mesh.faces) {
		getFaceData(face, &data);
	}
	return data;
}

FaceData pe::getTwoSidedFaceData(const Mesh& mesh) {

	FaceData data;
	for (Face* face : mesh.faces) {
		getFaceData(face, &data);
	}

	int size = data.vertices.size();
	for (int i = 0; i < size; i++) {
		/*
			Pushes vertices in reverse order to get clockwise order,
			which corresponds to the reverse face.
		*/
		data.vertices.push_back(data.vertices[size - i - 1]);
		/*
			Same for the other elements, but normals tangents and
			bitangents are also inverted.
		*/
		data.normals.push_back(-data.normals[size - i - 1]);
		data.tangents.push_back(-data.tangents[size - i - 1]);
		data.bitangents.push_back(-data.bitangents[size - i - 1]);
		data.uvCoordinates.push_back(data.uvCoordinates[size - i - 1]);
	}

	return data;
}

FaceData pe::getUniformFaceData(const Mesh& mesh) {
	FaceData data;
	for (Face* face : mesh.faces) {
		getUniformFaceData(face, &data);
	}
	return data;
}