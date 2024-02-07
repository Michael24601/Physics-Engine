
#ifndef POLYHEDRON2_H
#define POLYHEDRON2_H

#include "vector2D.h"
#include "rigidBody.h"
#include "boundingSphere.h"
#include <vector>
#include <algorithm>

#include "face.h"

namespace pe {

	struct Edge {
		std::pair<Vector3D, Vector3D> vertices;

		Edge() {}

		Edge(const Vector3D& first, const Vector3D& second) {
			vertices = std::make_pair(first, second);
		}
	};


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
			const std::vector<Vector3D>& localVertices
		) : body{ body },
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

		virtual std::vector<Edge> calculateEdges(
			const std::vector<Vector3D>& vertices
		) const = 0;


		virtual std::vector<Face> calculateFaces(
			const std::vector<Vector3D>& vertices
		) const = 0;


		void update() {
			globalVertices.clear();
			for (const Vector3D& vertex : localVertices) {
				globalVertices.push_back(
					body->transformMatrix.transform(vertex)
				);
			}

			edges = calculateEdges(globalVertices);
			faces = calculateFaces(globalVertices);
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


		virtual void setTextureMap() = 0;
	};
}

#endif