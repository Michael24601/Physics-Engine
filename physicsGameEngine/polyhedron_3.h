
#ifndef POLYHEDRON3_H
#define POLYHEDRON3_H

#include "vector2D.h"
#include "rigidBody.h"
#include "boundingSphere.h"
#include <vector>
#include <algorithm>


namespace pe {

	class Polyhedron {

	private:

		std::vector<std::vector<int>> faces;
		std::vector<std::pair<int, int>> edges;

		// The normal of each face
		std::vector<Vector3D> normals;

		// The centroid of each face
		std::vector<Vector3D> centroids;

	public:

		std::vector<Vector3D> localVertices;
		std::vector<Vector3D> globalVertices;
		RigidBody* body;

		BoundingSphere boundingSphere;

		Polyhedron(
			RigidBody* body,
			real mass,
			const Vector3D& position,
			const Matrix3x3& inertiaTensor,
			const std::vector<Vector3D>& localVertices,
			std::vector<std::pair<int, int>> edges,
			std::vector<std::vector<int>> faces
		) : body{ body },
			localVertices{ localVertices },
			edges{ edges },
			faces{faces},
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


		virtual void update() {
			globalVertices.clear();
			for (const Vector3D& vertex : localVertices) {
				globalVertices.push_back(
					body->transformMatrix.transform(vertex)
				);
			}

			// Here we update information related to faces and edges
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


	Vector3D calculateFaceNormal(
		std::vector<Vector3D>& vertices,
		std::vector<int>& indeces
	);
}

#endif