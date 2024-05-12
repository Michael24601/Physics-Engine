
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


std::vector<FaceData> pe::getFaceDataWithMaterials(
	const Polyhedron& polyhedron,
	std::vector<std::string>& materials
) {

	std::map<std::string, std::vector<FaceData>> textureFacesMap;

	// Grouping faces by their texture names
	for (Face* face : polyhedron.faces) {
		std::string textureName = face->texture;
		FaceData faceData;
		getFaceData(face, &faceData);
		textureFacesMap[textureName].push_back(faceData);
	}

	for (const auto& pair : textureFacesMap) {
		// Texture name vector
		materials.push_back(pair.first);
	}

	// Merging the FaceData vectors into a single vector in the order of materials
	std::vector<FaceData> faceDataVector(materials.size());
	for (int i = 0; i < materials.size(); i++) {
		std::vector<FaceData> faceDataForTexture = textureFacesMap[materials[i]];
		for (FaceData& data : faceDataForTexture) {
			faceDataVector[i].vertices.insert(
				faceDataVector[i].vertices.end(), data.vertices.begin(), 
				data.vertices.end()
			);
			faceDataVector[i].tangents.insert(
				faceDataVector[i].tangents.end(), data.tangents.begin(),
				data.tangents.end()
			);
			faceDataVector[i].bitangents.insert(
				faceDataVector[i].bitangents.end(), data.bitangents.begin(),
				data.bitangents.end()
			);
			faceDataVector[i].uvCoordinates.insert(
				faceDataVector[i].uvCoordinates.end(), data.uvCoordinates.begin(),
				data.uvCoordinates.end()
			);
			faceDataVector[i].normals.insert(
				faceDataVector[i].normals.end(), data.normals.begin(),
				data.normals .end()
			);
		}
	}

	return faceDataVector;
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

EdgeData pe::getOBBData(const Polyhedron& polyhedron) {

	EdgeData data;
	Vector3D halfsize = polyhedron.getOBBHalfsize();
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


EdgeData pe::getAABBData(const Polyhedron& polyhedron) {

	EdgeData data;
	Vector3D halfsize = polyhedron.getAABBHalfsize();
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


FaceData pe::getUniformFaceData(const Mesh& mesh) {
	FaceData data;
	for (Face* face : mesh.faces) {
		getUniformFaceData(face, &data);
	}
	return data;
}


bool pe::isAABBInFrustum(
	const Polyhedron& p, const glm::mat4& projectionViewMatrix
) {

	std::vector<Vector3D> globalVertices(p.localVertices.size());
	Matrix3x4 transform = p.getTransformMatrix();
	for (int i = 0; i < p.localVertices.size(); i++) {
		globalVertices[i] = transform.transform(p.localVertices[i]);
	}

	Vector3D aabbHalfSize;
	Vector3D aabbCentre;
	Polyhedron::calculateAxisAlignedBoundingBox(globalVertices, aabbHalfSize, aabbCentre);

	glm::vec4 centerClip = projectionViewMatrix * glm::vec4(
		convertToGLM(aabbCentre), 1.0f);

	float distanceToNearPlane = centerClip.z - centerClip.w;
	float distanceToFarPlane = centerClip.z + centerClip.w;

	glm::vec3 aabbExtents(
		aabbHalfSize.x * abs(projectionViewMatrix[0][0]),
		aabbHalfSize.y * abs(projectionViewMatrix[1][1]),
		aabbHalfSize.z * abs(projectionViewMatrix[2][2]));

	if (centerClip.x + aabbExtents.x < -centerClip.w || // Left plane
		centerClip.x - aabbExtents.x > centerClip.w ||  // Right plane
		centerClip.y + aabbExtents.y < -centerClip.w || // Bottom plane
		centerClip.y - aabbExtents.y > centerClip.w ||  // Top plane
		distanceToNearPlane > glm::max(aabbExtents.x, 
			glm::max(aabbExtents.y, aabbExtents.z)) || // Near plane
		distanceToFarPlane < -glm::max(aabbExtents.x, 
			glm::max(aabbExtents.y, aabbExtents.z))) { // Far plane
		// AABB is outside frustum
		return false;
	}

	// AABB is inside or intersects frustum
	return true;
}


bool pe::isBoundingSphereInFrustum(
	const Polyhedron& p, const glm::mat4& projectionViewMatrix
) {

	glm::vec3 worldCentre = convertToGLM(
		p.getTransformMatrix().transform(p.getSphereOffset())
	);
	float radius = p.getBoundingSphereRadius();

	glm::vec4 centerClip = projectionViewMatrix * glm::vec4(worldCentre, 1.0f);

	float distanceToNearPlane = centerClip.z - centerClip.w;
	float distanceToFarPlane = centerClip.z + centerClip.w;

	if (centerClip.x - radius > centerClip.w ||  // Right plane
		centerClip.x + radius < -centerClip.w || // Left plane
		centerClip.y - radius > centerClip.w ||  // Top plane
		centerClip.y + radius < -centerClip.w || // Bottom plane
		distanceToNearPlane > radius ||          // Near plane
		distanceToFarPlane < -radius) {          // Far plane

		// Sphere is outside frustum
		return false;
	}

	// Sphere is inside or intersects frustum
	return true;
}