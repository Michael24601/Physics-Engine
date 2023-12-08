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


#ifndef SHADER_INTERFACE_H
#define SHADER_INTERFACE_H

#include "polyhedron.h"
#include "solidSphere.h"
#include "cylinder.h"
#include "clothWithBungeeCord.h"
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
	glm::mat4 convertToGLM(const Matrix3x4& m);


	/*
		Returns a 3D vector in the glm format.
	*/
	glm::vec3 convertToGLM(const Vector3D& v);


	std::vector<std::vector<Vector3D>> triangulateFace(
		const std::vector<Vector3D>& vertices
	);


	edgeData getPolyhedronEdgeData(const Polyhedron& polyhedron);


	/*
		Polyhedron with no curved surfaces.
	*/
	faceData getPolyhedronFaceData(const Polyhedron& polyhedron);


	/*
		Returns the face data for a sphere.
		The difference is tha the normals of each vertex are calculated
		individually, instead of relying on the normal of the faces.
	*/
	faceData getSphereFaceData(const SolidSphere& sphere);


	/*
		Returns the face data for a cylinder.
		The vertices of the top and bottom circular faces have the afore-
		mentioned faces' normals, just like a polyhedron.
		On the other hand, the vertices of the tesselated strips making
		up the curved surface will have a normal similar to that of the
		normals of the vertices of a sphere. We can calculate it as the
		vector from the centroid of the circular face to the vertex on
		said circular face.
	*/
	faceData getCylinderFaceData(const Cylinder& cylinder);


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
    faceData getMeshFaceData(
        const ParticleMesh& mesh,
        int columnSize,
        int rowSize,
        order order
    );


    /*
        Also returns the face data, but the vertices calculate their normals
        individually, so it looks smooth. This function is specific to
		cloth.
    */
	faceData getSmoothMeshFaceData(
		const ClothWithBungeeCord& mesh,
		int columnSize,
		int rowSize,
		order order
	);

	/*
		Returns mesh edge data.
	*/
	edgeData getMeshEdgeData(const ParticleMesh& mesh);

}

#endif