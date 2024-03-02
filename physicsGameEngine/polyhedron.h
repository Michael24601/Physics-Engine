

#ifndef POLYHEDRON2_H
#define POLYHEDRON2_H

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
			const std::vector<Vector3D>& localVertices
		) : body{ new RigidBody() },
			localVertices{ localVertices },

			boundingSphere(
				body->position,
				findFurthestPoint().magnitude()
			) {
			body->setMass(mass);
			body->position = position;
			body->setInertiaTensor(inertiaTensor);

			globalVertices = localVertices;

			body->angularDamping = 1;
			body->linearDamping = 1;
			body->calculateDerivedData();
		}


		/*
			One reason why we have the child classes override the setEdges
			and setFaces functions, instead of just sending the vertex
			associations of the faces and edges to this parent class,
			is that we can't know if the faces are curved or straight, and
			if curved, what their normal vectors are (how curved they are).
			So it's required that the child classes create the Faces,
			and it follows that the edges be created in the same way.
		*/
		virtual void setEdges() = 0;


		virtual void setFaces() = 0;


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


		~Polyhedron() {
			delete body;
			for (int i = 0; i < faces.size(); i++) {
				delete faces[i];
			}
			for (int i = 0; i < edges.size(); i++) {
				delete edges[i];
			}
		}

	};
}

#endif