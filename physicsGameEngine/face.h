/*
	Header file for a face class. Faces form a mesh.
	Note that the face only holds the index associations for the vertices
	in the face, not the vertices themselves, as it would be wasteful to
	copy them, and inefficient in case they moved.

	However, it would be harmful to have a pointer to the mesh in the face,
	since the mesh itself has contains the face object. That would cause
	circular dependency, and make it impossible to copy the mesh and its
	faces. Instead, any function that requires access to the vertices of the
	mesh must pass the mesh as an argument.
*/

#ifndef FACE_H
#define FACE_H

#include "vector3D.h"
#include "vector2D.h"
#include "matrix3x4.h"
#include "matrix3x3.h"

namespace pe {

	// Forward declaration to avoid circular dependency
	class Mesh;

	/*
		The face class stores the association between vertices in a
		mesh class that make up face. It also stores basic face
		information like the normals and centroids.

		The mesh may be that of a rigid body, where the vertices are
		unchanging, and instead, the transformation matrix is sent to the
		GPU in order to redraw them when the body moves or rotates. In 
		such a case, the face is also uchanging, holding the local normal,
		centroid, and tangents of the faces.

		On the other hand, if the mesh is that of a soft body, which has
		not transform matrix and must have its vertices updated in real
		time for each frame, then the normal, tangent, and centroid, 
		can be updated by calling the calculation functions again.
	*/
	class Face {

	protected:

		Vector3D normal;
		Vector3D centroid;

		/*
			uv-coordinates: used to map a texture to the face. Each
			Vector2D object represents the coordinate ofa face vertex
			onto the texture, which is often an image with coordinates
			(0, 0) at the bottom left and (1, 1) at the top right.
		*/
		std::vector<Vector2D> textureCoordinates;


		bool findUniqueVertexIndexes(const Mesh* mesh, int index[3]) const;


		Vector3D calculateNormal(const Mesh* mesh) const; 


		Vector3D calculateCentroid(const Mesh* mesh) const; 


		/*
			Indexes of the vertices corresponding to the face. They have
			to be given in either clockwise or counter-clockwise order, as
			long as they are ordered. For this engine the order must always
			be counter-clockwise.
		*/
		std::vector<int> indexes;

	public:

		Face() {}


		Face(const Mesh* mesh, const std::vector<int>& indeces);


		void update(const Mesh* mesh);


		/*
			Returns the vertex at a certain index.
		*/
		Vector3D getVertex(const Mesh* mesh, int index) const;


		Vector3D getNormal() const;


		Vector3D getCentroid() const;


		int getIndex(int index) const;


		inline int getVertexCount() const;


		virtual void setTextureCoordinates(
			const std::vector<Vector2D>& textureCoordinates
		);


		Vector2D getTextureCoordinate(int index) const;


		/*
			Checks if the point is inside the boundaries of the face using a
			winding number algorithm.
			The winding number algorithm counts how many times a ray starting
			from the point intersects with the edges of the face.
			If the number is odd, the point is inside the face; if it's even,
			the point is outside.
			Note that depending on wether or not the vertices of the mesh are
			in local or global coordinates, the point may have to be
			modified first to be in that same basis.
		*/
		bool containsPoint(const Mesh* mesh, const Vector3D& point, int rayLength) const;
	};
}


#endif