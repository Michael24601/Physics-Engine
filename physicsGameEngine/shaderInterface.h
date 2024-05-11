
#ifndef SHADER_INTERFACE_H
#define SHADER_INTERFACE_H

#include "polyhedron.h"
#include "mesh.h"
#include <vector>
#include <map>
#include "collisionBox.h"

namespace pe {

	// Data needed in order to draw a face
	struct FaceData {
		/*
			The vertices of the faces, arranged by first extracting the
			faces from the polyhedron, then triangulating the faces, then
			flattening the data so that each triangle is represented by
			3 consecutive Vector objects in the array.
		*/
		std::vector<glm::vec3> vertices;

		/*
			The normals of each vertex (not in the polyhedron, but in the
			above array). The normal at each vertex instance (each vertex
			will appear in different faces/triangle triplets more than once)
			will depend on the shape being drawn. For your average
			polyhedron, it's the normal of the current face it belongs to,
			but for a sphere, or a cylinder, the normal of each vertex has
			to be calculated differently and individually.
		*/
		std::vector<glm::vec3> normals;

		/*
			Likewise, these are the tangents and bitangents, which can also
			be uniform on a face or unique to each vertex on the face.
		*/
		std::vector<glm::vec3> tangents;
		std::vector<glm::vec3> bitangents;

		/*
			The uv-coordinates of each vertex in each face, used for
			texture mapping.
			We use a 3D instead of a 2D vector for the coordinates as it
			makes it easier to group with others.
		*/
		std::vector<glm::vec3> uvCoordinates;
	};


	// Data needed to draw an edge
	struct EdgeData {
		/*
			The vertices of the faces, arranged in such a way that every
			two consecutive vector objects represent one edge in a
			polyehdron.
		*/
		std::vector<glm::vec3> vertices;
	};


	struct FrameVectors {
		/*
			The frame vectors are the normals, tangents, and bitangents,
			which can either be uniform across a face (flat surface),
			or vary from vertex to vertex (curved surface).

			The follow vectors contain lines of a certain length representing
			each value, all stemming from the centroid of each polyhedron's
			face.

			The vectors alternate between the centroid (base of the vector)
			and a point somewhere along its length.
		*/
		std::vector<glm::vec3> normals;
		std::vector<glm::vec3> tangents;
		std::vector<glm::vec3> bitangents;
	};


	/*
		Returns a homogeneous matrix in the glm format.
	*/
	glm::mat4 convertToGLM(const Matrix3x4& m);


	/*
		Returns a 3D vector in the glm format.
	*/
	glm::vec3 convertToGLM(const Vector3D& v);

	glm::vec2 convertToGLM(const Vector2D& v);


	// Appends the data to it
	void getEdgeData(Edge* edge, EdgeData* data);


	/*
		Returns the polyhedron's frame vectors: normals, tangents, and
		bitangents, for each face.
	*/
	void getUniformFrameVectors(
		Face* face,
		real length,
		FrameVectors* data
	);


	/*
		Returns the polyhedron's frame vectors: normals, tangents, and
		bitangents, for each vertex.
	*/
	void getFrameVectors(
		Face* face,
		real length,
		FrameVectors* data
	);

	/*
		Uses the individual vertex normals.
	*/
	void getFaceData(Face* face, FaceData* data);


	/*
		Uses the uniform face normals.
		This won't differ for flat faced polyhedrons, but will return
		a tessalated looking curved surface for curved polyhedra.
	*/
	void getUniformFaceData(Face* face, FaceData* data);

	/*
		Functions that can draw the whole Polyhedron.
	*/

	EdgeData getEdgeData(const Polyhedron& polyhedron);

	FrameVectors getUniformFrameVectors(
		const Polyhedron& polyhedron,
		real length
	);

	FrameVectors getFrameVectors(
		const Polyhedron& polyhedron,
		real length
	);


	EdgeData getCollisionBoxData(const Polyhedron& polyhedron);

	FaceData getFaceData(const Polyhedron& polyhedron);

	/*
		This function is used when there may be more than one texture used
		per polyhedron, and we want to seperate the faces based on the
		material.
	*/
	std::vector<FaceData> getFaceDataWithMaterials(
		const Polyhedron& polyhedron, 
		std::vector<std::string>& materials
	);

	FaceData getUniformFaceData(const Polyhedron& polyhedron);

	/*
		Functions that can draw the whole Mesh.
	*/

	EdgeData getEdgeData(const Mesh& mesh);

	FrameVectors getUniformFrameVectors(
		const Mesh& mesh,
		real length
	);

	FrameVectors getFrameVectors(
		const Mesh& mesh,
		real length
	);

	FaceData getFaceData(const Mesh& mesh);

	FaceData getUniformFaceData(const Mesh& mesh);

	/*
		Checks if the given object is visible to the given projection
		view matrix (projection * view) by checking if the bounding box
		of the object is within or intersects the frustum of the matrix.

		This is done to cull objects not visible to the camera, improving
		performance.
	*/
	bool isBoundingBoxInFrustum(
		const Polyhedron& p, const glm::mat4& projectionViewMatrix
	);

	/*
		Checks if the given object is visible to the given projection
		view matrix (projection * view) by checking if the bounding sphere
		of the object is within or intersects the frustum of the matrix.

		This is done to cull objects not visible to the camera, improving
		performance.
	*/
	bool isBoundingSphereInFrustum(
		const Polyhedron& p, const glm::mat4& projectionViewMatrix
	);

}

#endif