/*
	
*/

#ifndef FACE_H
#define FACE_H

#include "vector3D.h"
#include "vector3D.h"
#include "matrix3x4.h"
#include <vector>

namespace pe {


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
	*/
	class Face {

	protected:

		std::vector<Vector3D>* localVertices;
		std::vector<Vector3D>* globalVertices;
		std::vector<int> indeces;

		Vector3D calculateLocalNormal() {

			Vector3D firstVertex = (*localVertices)[indeces[0]];
			Vector3D secondVertex;
			Vector3D thirdVertex;
			bool flag = false;
			for (int i = 1; i < getVertexNumber(); i++) {
				if (!flag && (*localVertices)[indeces[i]] != firstVertex) {
					secondVertex = (*localVertices)[indeces[i]];
					flag = true;
				}
				else if ((*localVertices)[indeces[i]] != firstVertex
					&& (*localVertices)[indeces[i]] != secondVertex) {
					thirdVertex = (*localVertices)[indeces[i]];
					break;
				}
			}

			Vector3D AB = secondVertex - firstVertex;
			Vector3D AC = thirdVertex - firstVertex;;
			Vector3D normal = AB.vectorProduct(AC);
			normal.normalize();
			return normal;
		}

		Vector3D calculateLocalCentroid() {
			Vector3D sum;
			for (int i = 0; i < getVertexNumber(); i++) {
				sum += (*localVertices)[indeces[i]];
			}
			Vector3D centroid = sum * (
				1.0f / static_cast<real>(getVertexNumber())
				);
			return centroid;
		}

	public:

		Vector3D normal;
		Vector3D localNormal;
		Vector3D centroid;
		Vector3D localCentroid;

		std::vector<Vector2D> textureCoordinates;


		Face() {}


		Face(
			std::vector<Vector3D>* localVertices,
			std::vector<Vector3D>* globalVertices,
			std::vector<int> indeces
		) : localVertices{ localVertices },
			globalVertices{ globalVertices },
			indeces{ indeces } {

			localNormal = calculateLocalNormal();
			localCentroid = calculateLocalCentroid();

			/*
				Initially, the normaland centroid are the same as the local
				ones.
			*/
			normal = localNormal;
			centroid = localCentroid;
		}


		Vector3D& getVertex(int index) const {
			return (*globalVertices)[indeces[index]];
		}


		inline int getVertexNumber() const {
			return indeces.size();
		}


		virtual void update(const Matrix3x4& transformMatrix) {
			normal = transformMatrix.transform(localNormal);
			normal.normalize();
			centroid = transformMatrix.transform(localCentroid);
		}
	};
}


#endif