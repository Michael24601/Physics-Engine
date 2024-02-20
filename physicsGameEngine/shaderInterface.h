
#ifndef SHADER_INTERFACE_H
#define SHADER_INTERFACE_H

#include "polyhedron.h"
#include "particleMesh.h"
#include <vector>

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
		*/
		std::vector<glm::vec2> uvCoordinates;
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


	EdgeData getPolyhedronEdgeData(const Polyhedron& polyhedron);


	/*
		Returns the polyhedron's frame vectors: normals, tangents, and
		bitangents, for each face.
	*/
	FrameVectors getPolyhedronUniformFrameVectors(
		const Polyhedron& polyhedron,
		real length
	);


	/*
		Returns the polyhedron's frame vectors: normals, tangents, and
		bitangents, for each vertex.
	*/
	FrameVectors getPolyhedronFrameVectors(
		const Polyhedron& polyhedron,
		real length
	);


	/*
		Uses the individual vertex normals.
	*/
	FaceData getPolyhedronFaceData(const Polyhedron& polyhedron);
	

	/*
		Uses the uniform face normals.
		This won't differ for flat faced polyhedrons, but will return
		a tessalated looking curved surface for curved polyhedra.
	*/
	FaceData getUniformPolyhedronFaceData(const Polyhedron& polyhedron);


	EdgeData getMeshEdgeData(const ParticleMesh& mesh);


	FrameVectors getMeshUniformFrameVectors(
		const ParticleMesh& mesh,
		real length
	);


	FrameVectors getMeshFrameVectors(
		const ParticleMesh& mesh,
		real length
	);


	FaceData getUniformMeshFaceData(const ParticleMesh& mesh);


	FaceData getMeshFaceData(const ParticleMesh& mesh);

}

#endif SHADER_INTERFACE_H