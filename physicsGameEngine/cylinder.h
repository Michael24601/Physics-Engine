
#ifndef CYLINDER_H
#define CYLINDER_H

#include "polyhedron.h"
#include "tesselationUtil.h"

namespace pe {

	class Cylinder : public Polyhedron {

	public:

		real radius;
		real length;
		int segments;

		Cylinder(
			RigidBody* body, 
			real radius, 
			real length, 
			real mass, 
			int segments, 
			Vector3D position) :
			Polyhedron(
				body,
				mass,
				position,
				Matrix3x3(
					(1.0 / 12.0) * mass *
					(3.0 * radius * radius + length * length), 0, 0,
					0, (1.0 / 12.0) * mass * 
					(3.0 * radius * radius + length * length), 0,
					0, 0, (1.0 / 12.0)* mass* (3.0 * radius * radius)
				),
				generateCylinderVertices(
					Vector3D(0, 0, 0),
					radius,
					length,
					segments
				)
			),
			radius{ radius }, length{length}, segments{ segments } {}


		virtual std::vector<Face> calculateFaces(
			const std::vector<Vector3D>& vertices
		) const override {

			std::vector<Face> faces;

			std::vector<std::vector<Vector3D>> vertexFaces =
				returnCylinderFaces(vertices, segments);

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
				returnCylinderEdges(vertices, segments);

			for (int i = 0; i < vertexEdges.size(); i++) {
				Edge edge(vertexEdges[i].first, vertexEdges[i].second);
				edges.push_back(edge);
			}

			return edges;
		}
	};
}


#endif