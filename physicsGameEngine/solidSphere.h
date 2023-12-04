
#ifndef SOLID_SPHERE_H
#define SOLID_SPHERE_H

#include "polyhedron.h"
#include "tesselationUtil.h"

namespace pe {

	class SolidSphere : public Polyhedron {

	public:

		real radius;
		int latitudeSegments;
		int longitudeSegments;

		SolidSphere(RigidBody* body, real radius, real mass, int latitudeSegments,
			int longtitudeSegments, Vector3D position) :
			Polyhedron(
				body,
				mass,
				position,
				Matrix3x3(
					(2.0 / 5.0) * mass * radius * radius, 0, 0,
					0, (2.0 / 5.0) * mass * radius * radius, 0,
					0, 0, (2.0 / 5.0) * mass * radius * radius
				),
				generateSphereVertices(
					Vector3D(0, 0, 0),
					radius,
					latitudeSegments,
					longtitudeSegments
				)
			),
			radius{ radius }, latitudeSegments{ latitudeSegments }, 
			longitudeSegments{ longtitudeSegments } {}


		virtual std::vector<Face> calculateFaces(
			const std::vector<Vector3D>& vertices
		) const override {

			std::vector<Face> faces;

			std::vector<std::vector<Vector3D>> vertexFaces = 
				returnTesselatedFaces(vertices, latitudeSegments, 
					longitudeSegments);

			for (int i = 0; i < vertexFaces.size(); i++) {
				Face face(vertexFaces[i]);
				faces.push_back(face);
			}

			return faces;
		}

		virtual std::vector<Edge> calculateEdges(
			const std::vector<Vector3D>& vertices
		) const override {

			std::vector<Edge> edges;

			std::vector<std::pair<Vector3D, Vector3D>> vertexEdges =
				returnTesselatedEdges(vertices, latitudeSegments,
					longitudeSegments);

			for (int i = 0; i < vertexEdges.size(); i++) {
				Edge edge(vertexEdges[i].first, vertexEdges[i].second);
				edges.push_back(edge);
			}

			return edges;
		}
	};
}


#endif