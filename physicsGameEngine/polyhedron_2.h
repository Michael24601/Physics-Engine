

#ifndef POLYHEDRON2_H
#define POLYHEDRON2_H

#include "vector2D.h"
#include "rigidBody.h"
#include "boundingSphere.h"
#include <vector>
#include <algorithm>

#include "face.h"
#include "edge.h"

namespace pe {

	class Polyhedron {

	public:

		std::vector<Vector3D> localVertices;
		RigidBody* body;

		std::vector<Face> faces;
		std::vector<Edge> edges;
		std::vector<Vector3D> globalVertices;

		BoundingSphere boundingSphere;

		Polyhedron(
			RigidBody* body,
			real mass,
			const Vector3D& position,
			const Matrix3x3& inertiaTensor,
			const std::vector<Vector3D>& localVertices,
			const std::vector<std::vector<int>>& faceIndexes,
			const std::vector<std::pair<int, int>>& edgeIndexes
		) : body{ body },
			localVertices{ localVertices },

			boundingSphere(
				body->position,
				findFurthestPoint().magnitude()
			) {
			body->setMass(mass);
			body->position = position;
			body->setInertiaTensor(inertiaTensor);

			this->globalVertices = localVertices;

			body->angularDamping = 1;
			body->linearDamping = 1;
			body->calculateDerivedData();

			// Creates the face and edge objects from the associations
			for (int i = 0; i < faceIndexes.size(); i++) {
				faces.push_back(Face(
					(this->localVertices), 
					&(this->globalVertices), 
					faceIndexes[i]
				));
			}
			for (int i = 0; i < edgeIndexes.size(); i++) {
				edges.push_back(Edge(
					&(this->localVertices),
					&(this->globalVertices),
					edgeIndexes[i].first,
					edgeIndexes[i].second
				));
			}
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
			for (Face& face : faces) {
				face.update(body->transformMatrix);
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

	};
}

#endif