

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


		virtual void setTextureMap() = 0;


		/*
			Returns the polyhedron data:
			The vertices of each face in order (not each unique vertex).
			The normal at each vertex of each face (in order), which
			correspond to the normals of each face.
		*/
		virtual void getPolyhedronData(
			std::vector<Vector3D>* vertices,
			std::vector<Vector3D>* normals
		) const {
			for (const Face& face : faces) {
				for (int i = 0; i < face.getVertexNumber(); i++) {
					vertices->push_back(face.getVertex(i));
					normals->push_back(face.getNormal());
				}
			}
		}
	};
}

#endif