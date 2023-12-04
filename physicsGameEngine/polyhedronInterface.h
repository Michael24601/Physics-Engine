/*
	This file provides functions that essentially prepare data gotten from
	the polyhedron class for the graphic module.
	This file uses the global positions to provide opengl with the vertex
	data, since the global positions have to be calculated anyway in the
	Polyhedron class for other physics related modules.
	The shaders still usually expect the model (transform to world
	coordinates), view (camera), and projection (perspective) matrices.
	In our case, if the data came from this file, the first of the three
	matrices should be the identity matrix, as the vertices are already
	in world coordinates.
*/


#ifndef POLYHEDRON_INTERFACE_H
#define POLYHEDRON_INTERFACE_H

#include "polyhedron.h"
#include <vector>

namespace pe {

	// Data needed in order to draw a face
	struct faceData {
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
	struct edgeData {
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
	glm::mat4 convertToGLM(const Matrix3x4& m) {
		return glm::mat4(
			m.data[0], m.data[4], m.data[8], 0.0f,
			m.data[1], m.data[5], m.data[9], 0.0f,
			m.data[2], m.data[6], m.data[10], 0.0f,
			m.data[3], m.data[7], m.data[11], 1.0f
		);
	}


	/*
		Returns a 3D vector in the glm format.
	*/
	glm::vec3 convertToGLM(const Vector3D& v) {
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


	edgeData getPolyhedronEdgeData(const Polyhedron& polyhedron) {
		std::vector<glm::vec3> flattenedPositions;

		std::vector<Edge> edges = polyhedron.edges;

		for (const auto& edge : edges) {
			flattenedPositions.push_back(convertToGLM(edge.vertices.first));
			flattenedPositions.push_back(convertToGLM(edge.vertices.second));
		}
		edgeData data{ flattenedPositions };
		return data;
	}


	/*
		Polyhedron with no curved surfaces.
	*/
	faceData getPolyhedronFaceData(const Polyhedron& polyhedron) {
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

		faceData data{flattenedPositions, flattenedNormals};
		return data;
	}
}

#endif