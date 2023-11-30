
#ifndef SOLID_SPHERE_H
#define SOLID_SPHERE_H

#include "primitive.h"
#include "drawingUtil.h"

namespace pe {

	class SolidSphere : public Primitive {

	public:

		real radius;
		int latitudeSegments;
		int longitudeSegments;

		SolidSphere(RigidBody* body, real radius, real mass, int latitudeSegments,
			int longtitudeSegments, Vector3D position) : Primitive(body, mass, position),
			radius{ radius }, latitudeSegments{ latitudeSegments }, 
			longitudeSegments{ longtitudeSegments } {

			real inertiaScalar = (2.0 / 5.0) * mass * radius * radius;
			
			pe::Matrix3x3 inertiaTensor(
				inertiaScalar, 0, 0,
				0, inertiaScalar, 0,
				0, 0, inertiaScalar
			);

			// Sets vertices using tessalation
			localVertices = generateSphereVertices(position, radius,
				latitudeSegments, longitudeSegments);
			globalVertices.resize(localVertices.size());

			body->setInertiaTensor(inertiaTensor);

			body->angularDamping = 1;
			body->linearDamping = 1;

			body->calculateDerivedData();
			updateVertices();

			// Sets the face and edge connection
			setEdges();
			setFaces();
			setLocalEdges();
			setLocalFaces();
		}

		// Connects the correct edges using local coordinates
		virtual void setFaces() override {
			std::vector<std::vector<Vector3D*>> vertexFaces = 
				returnTesselatedFacesPointers(globalVertices, latitudeSegments, 
					longitudeSegments);

			for (int i = 0; i < vertexFaces.size(); i++) {
				Face face;
				for (int j = 0; j < vertexFaces[i].size(); j++) {
					face.vertices.push_back(vertexFaces[i][j]);
				}
				faces.push_back(face);
			}
		}

		virtual void setEdges() override {
			std::vector<std::pair<Vector3D*, Vector3D*>> vertexEdges =
				returnTesselatedEdgesPointers(globalVertices, latitudeSegments,
					longitudeSegments);

			for (int i = 0; i < vertexEdges.size(); i++) {
				Edge edge;
				edge.vertices = { vertexEdges[i].first, vertexEdges[i].second };
				edges.push_back(edge);
			}
		}

		virtual void setLocalFaces() override {
			std::vector<std::vector<Vector3D*>> vertexFaces =
				returnTesselatedFacesPointers(localVertices, latitudeSegments,
					longitudeSegments);

			for (int i = 0; i < vertexFaces.size(); i++) {
				Face face;
				for (int j = 0; j < vertexFaces[i].size(); j++) {
					face.vertices.push_back(vertexFaces[i][j]);
				}
				localFaces.push_back(face);
			}
		}

		virtual void setLocalEdges() override {
			std::vector<std::pair<Vector3D*, Vector3D*>> vertexEdges =
				returnTesselatedEdgesPointers(localVertices, latitudeSegments,
					longitudeSegments);

			for (int i = 0; i < vertexEdges.size(); i++) {
				Edge edge;
				edge.vertices = { vertexEdges[i].first, vertexEdges[i].second };
				localEdges.push_back(edge);
			}
		}
	};
}


#endif