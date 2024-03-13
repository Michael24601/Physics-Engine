/*
	Header file for a face class. Faces form a polyhedron.
*/

#ifndef FACE_H
#define FACE_H

#include "vector3D.h"
#include "vector2D.h"
#include "matrix3x4.h"
#include "matrix3x3.h"
#include <vector>

namespace pe {


	/*
		Vertices, normals, etc... can be in the global (world) coordinates
		of the Polyhedron or the local (relative to the body centre of 
		gravity) coordinates of the Polyhedron.
		There is technically another basis we may use, which is the Face
		basis, where the vertices are relative to the centroid, and the
		normal is the z axis, the tangent is the x axis, and the bitangent
		is the y axis. However, because of the ease of calculating the
		vertices in the face basis, we don't include it in the enum.
	*/
	enum class Basis {
		LOCAL,
		GLOBAL
	};


	/*
		The face class stores the association between vertices in a
		polyhedron class that make up face. It also stores basic face
		information like the normals and centroids.

		This brings us to some tradeoffs we need to consider for the class
		design:

			- One tradeoff is about the way we store the vertex association
			in the face. We assume there exists a Polyhedron object with
			all of its vertices in two vectors: local and global vertices.
			In the first method, we store copies of the vertices associated
			with the face in the class; this won't work as it is too slow,
			requiring us to copy the vertices each frame (as the global
			vertices change each frame). This also requires that the
			Polyhedron class carry the information of which vertices belong
			to which faces, which really should be Face information.
			We can direguard this function. The second method is to store
			vectors of pointers to the (local and global) vertices
			associated with a face in the Face class. This approach is fast,
			but not memory efficient, as storing many 8-byte pointers can be
			expensive, and volatile, as we can't know that when the global
			vertices are updated each frame, the vector isn't cleared and
			filled again, changing the memory addresses.
			The third method is to store pointers to the global and local
			vertex vectors directly, which should be cheap, as well as a
			vector of integers for the indexes of the vectors that are
			associated with the face. This is memory efficient, as storing
			integers is cheap, and is more robust, but it also requires
			two layers of indirection, with (*vectorOfVertices)[indexes[i]]
			being a vertex in the face. We go with the third method.

			- Another tradeoff to consider is wether or not we want to
			store the centroids and normals, or wether we calculate them
			on the fly in functions without having datafields for them.
			The first method is usually faster when we use the normals and
			centroids several times, as they only need to be calculated
			once, regardless of how many times they're used. The second
			method on the other hand, is more memory efficient, as we don't
			permanently store the normal and centroid, but it ends up being
			slower is the values have to be used several times. We go with
			the first method for this class, as the normals and centroid
			may be used in the collision detection, resolution, and graphics
			modules all at once in any single frame.

			- Speaking of calculating normals and centroids, one tradeoff
			has to do with how the global (non local) normals and centroids
			are calculated each frame. In the first method, we use the
			centroid and normal calculation algorithms with the global
			vertices instead of local ones. In the second method, we only
			use the centroid and normal algorithms on the local vertices
			once, and then use the transform matrix of the polyhedron to
			get the global version each frame. The first method grows in
			complexity the more vertices we have, and becomes slower, while
			the second method has constant speed, and is faster when we
			have a lot of vertices. Note that the second function requires
			normalization of the normal after transformation, though this
			operation is also constant. Another advantage of the second
			method: we won't always have straight faces, so for curved faces
			the normal at each vertex in the face does not necessarily have
			the normal of the face. Thus, we will need to tell the Face
			class each time how to calculate the vertex normals, and having
			the flexibility to calculate the global and local versions
			may be too complex in certain cases. One disadvantage of the
			second method is that the localNormal and localCentroid now
			have to be stored as well, which takes up a bit more space.
			The second method is still better however, so we go with it.
			That being said, we still keep the first method as an
			option (we can calculate values like tangents and normals 
			locally or globally, because if say, this class was used 
			instead by a deformable object like cloth, then we wouldn't
			have a transform matrix anymore to use.

		Also note that we assume the face is convex, as many formulas and
		functions only work with that assumption.
	*/
	class Face {

	protected:

		std::vector<Vector3D>* localVertices;
		std::vector<Vector3D>* globalVertices;
		
		/*
			Indexes of the vertices corresponding to the face. They have
			to be given in either clockwise or counter-clockwise order, as
			long as they are ordered. For this engine the order must always
			be counter-clockwise.
		*/
		std::vector<int> indeces;

		Vector3D normal;
		Vector3D localNormal;
		Vector3D centroid;
		Vector3D localCentroid;

		Vector3D tangent;
		Vector3D localTangent;
		Vector3D bitangent;
		Vector3D localBitangent;

		/*
			uv-coordinates: used to map a texture to the face. Each
			Vector2D object represents the coordinate ofa face vertex
			onto the texture, which is often an image with coordinates
			(0, 0) at the bottom left and (1, 1) at the top right.
		*/
		std::vector<Vector2D> textureCoordinates;


		/*
			Because tesselation may cause some vertices to be degenerate,
			we need a function that returns distinct vertices.
			This function returns the first distinct vertex indices to
			either side of the given index's vertex.
			We assume any face has at least 3-non degenerate indices.
		*/
		void findDistinctVertices(
			int givenIndex,
			int& firstIndex,
			int& secondIndex
		) const;


		Vector3D calculateNormal(Basis basis) const; 


		Vector3D calculateCentroid(Basis basis) const; 


		Vector3D calculateTangent(Basis basis) const;


		Vector3D calculateBitangent(Basis basis) const;


		/*
			Finds the furthest vertex index from the centroid.
		*/
		int getFurthestPoint() const;


		/*
			Sets the Uv coordinate for any face.
			Assumes the centroid will have (0.5, 0.5) as a coordinate.
			Finds the furthest point from the centroid, and places it
			in the furthest possible location that guarantees other
			points are within the (0, 0) to (1, 1) range.
			This works very welll when the vertices are around the same
			distance from the centroid, and works less well when there
			are far away outliers and closely clustered vertices that
			skew the centroid's position.
		*/
		std::vector<Vector2D> setUvCoordinates() const;


	public:

		Face() {}


		Face(
			std::vector<Vector3D>* localVertices,
			std::vector<Vector3D>* globalVertices,
			std::vector<int>& indeces
		);


		/*
			Returns the local or global vertex at a certain index.
			The basis is global by default.
		*/
		Vector3D getVertex(int index, Basis basis = Basis::GLOBAL) const;


		Vector3D getNormal(Basis basis = Basis::GLOBAL) const;


		Vector3D getCentroid(Basis basis = Basis::GLOBAL) const;


		Vector3D getTangent(Basis basis = Basis::GLOBAL) const;


		Vector3D getBitangent(Basis basis = Basis::GLOBAL) const;


		/*
			Returns the normal of a vertex in the face. Since the
			face is assumed to be flat, that turns out to be the same
			as the face's normal. So the index won't change anything
			in this face, but if the class were to be extended and
			become curved, the normals wouldn't be uniform.
		*/
		virtual Vector3D getVertexNormal(
			int index, Basis basis = Basis::GLOBAL
		) const;


		virtual Vector3D getVertexTangent(
			int index, Basis basis = Basis::GLOBAL
		) const;


		virtual Vector3D getVertexBitangent(
			int index, Basis basis = Basis::GLOBAL
		) const;


		inline int getVertexNumber() const;


		std::vector<Vector3D> getVertices();


		/*
			Transforms values like the local normal and tangent using some
			transform matrix.
		*/
		virtual void update(const Matrix3x4& transformMatrix);


		void setTextureCoordinates(
			std::vector<Vector2D>& textureCoordinates
		);


		Vector2D getTextureCoordinate(int index) const;


		int getIndex(int index);


		/*
			Check if the point is inside the boundaries of the face using a
			winding number algorithm.
			The winding number algorithm counts how many times a ray starting
			from the point intersects with the edges of the face.
			If the number is odd, the point is inside the face; if it's even,
			the point is outside.
		*/
		bool containsPoint(const Vector3D& point) const;
	};
}


#endif