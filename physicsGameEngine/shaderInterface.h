
#ifndef SHADER_INTERFACE_H
#define SHADER_INTERFACE_H

#include "polyhedron.h"
#include "cloth.h"
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


	/*
		Returns a homogeneous matrix in the glm format.
	*/
	glm::mat4 convertToGLM(const Matrix3x4& m);


	/*
		Returns a 3D vector in the glm format.
	*/
	glm::vec3 convertToGLM(const Vector3D& v);


	EdgeData getPolyhedronEdgeData(const Polyhedron& polyhedron);


	/*
		Returns each face's normal vector for drawing, from the face
		centroid, with the specified length in the normal's direction.
	*/
	EdgeData getPolyhedronFaceNormalsData(
		const Polyhedron& polyhedron,
		real length
	);


	/*
		Returns each vertex's normal. If the faces are straight, that
		normal matches the faces', but if the faces are curved, the
		normals differ from vertex to vertex on the same face.
	*/
	EdgeData getPolyhedronVertexNormalsData(
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


	/*
		Returns individual vertex index in particle mesh.
	*/
	glm::vec3 calculateMeshVertexNormal(
		const std::vector<Particle>& particles,
		int targetIndex,
		int columnSize,
		int rowSize
	);


	/*
		Returns the face data for a mesh. Note that the normals used
		for each vertex is the face normal, so it will look triangulated.
		We can specify if we want the faces in clockwise or counter-clockwise
		order.
	*/
	FaceData getMeshFaceData(
		const ParticleMesh& mesh,
		int columnSize,
		int rowSize,
		Order order
	);


	/*
		Returns mesh edge data.
	*/
	EdgeData getMeshEdgeData(const ParticleMesh& mesh);


	/*
		Also returns the face data, but the vertices calculate their normals
		individually, so it looks smooth. This function is specific to
		cloth.
	*/
	FaceData getSmoothMeshFaceData(
		const Cloth& mesh,
		int columnSize,
		int rowSize,
		Order order
	);
}

#endif SHADER_INTERFACE_H