/*
	Header file for the general polyhedron class.
*/

#ifndef POLYHEDRON_H
#define POLYHEDRON_H

#include "vector2D.h"
#include "rigidBody.h"
#include "boundingSphere.h"
#include <vector>
#include <algorithm>

#include "face.h"
#include "curvedFace.h"
#include "edge.h"

namespace pe {

	class Polyhedron {

	public:

		std::vector<Vector3D> localVertices;
		RigidBody* body;

		std::vector<Face*> faces;
		std::vector<Edge*> edges;
		std::vector<Vector3D> globalVertices;

		BoundingSphere boundingSphere;

		Polyhedron(
			real mass,
			const Vector3D& position,
			const Matrix3x3& inertiaTensor,
			const std::vector<Vector3D>& localVertices,
			RigidBody* body
		) : body{ body },
			localVertices{ localVertices },
			boundingSphere(
				body->position,
				findFurthestPoint().magnitude()
			)
		{
			body->setMass(mass);
			body->position = position;
			body->setInertiaTensor(inertiaTensor);

			globalVertices = localVertices;

			body->angularDamping = 1;
			body->linearDamping = 1;
			body->calculateDerivedData();
		}


		void update() {
			globalVertices.clear();
			for (const Vector3D& vertex : localVertices) {
				globalVertices.push_back(
					body->transformMatrix.transform(vertex)
				);
			}

			/*
				Because we use pointers to the vectors of vertices in the
				Polyhedron class in the Faces and Edges, we don't need
				to update them. However, faces also have normals and
				centroids which need to be updated here.
			*/
			for (Face* face : faces) {
				face->update(body->transformMatrix);
			}
		}


		Vector3D findFurthestPoint() const {

			Vector3D furthestPoint = *std::max_element(
				localVertices.begin(),
				localVertices.end(),
				[](const Vector3D& first, const Vector3D& second) -> bool {
					return first.magnitudeSquared()
						< second.magnitudeSquared();
				}
			);
			return furthestPoint;
		}

		/*
		~Polyhedron() {
			delete body;
			for (int i = 0; i < faces.size(); i++) {
				delete faces[i];
			}
			for (int i = 0; i < edges.size(); i++) {
				delete edges[i];
			}
		}
		*/


		/*
			One design issue is wether we create setters for the face
			and edge associations, or we create pure virtual functions
			that need to be implemented in subclasses which force the
			edges and faces to be set.
			We go with the former approach as it provides the flexibility
			of instantiating a polyhedron object without having to have
			a suitable subclass.
		*/
		void setFaces(std::vector<Face*> faces) {
			this->faces = faces;
		}


		void setEdges(std::vector<Edge*> edges) {
			this->edges = edges;
		}


		Vector3D getAxis(int index) const {
			return body->transformMatrix.getColumnVector(index);
		}


		Vector3D getCentre() const {
			return body->position;
		}


		Vector3D getFaceNormal(int index) const {
			return faces[index]->getNormal();
		}


		/*
			Function used in the GJK collision detection and generation
			algorithm; it finds the furthest point from a given direction
			in the Polyhedron.
		*/
		Vector3D support(const Vector3D& direction) const {
			// Initialize with the first vertex
			Vector3D furthestVertex = globalVertices[0];
			real maxDot = furthestVertex.scalarProduct(direction);

			// Iterate through all vertices to find the one farthest in the given direction
			for (size_t i = 1; i < globalVertices.size(); ++i) {
				real dotProduct = globalVertices[i].scalarProduct(direction);
				if (dotProduct > maxDot) {
					maxDot = dotProduct;
					furthestVertex = globalVertices[i];
				}
			}

			return furthestVertex;
		}
	};
}

#endif