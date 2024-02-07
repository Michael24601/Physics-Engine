
#ifndef CONE_H
#define CONE_H

#include "polyhedron.h"
#include "tesselationUtil.h"

namespace pe {
		
	class Cone : public Polyhedron {

	public:

		real radius;
		real length;
		int segments;

		Cone(
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
					(3.0 / 80.0)* mass* (radius* radius + 4.0 * length * length), 0, 0,
					0, (3.0 / 80.0)* mass* (radius* radius + 4.0 * length * length), 0,
					0, 0, (3.0 / 40.0)* mass* radius* radius
				),
				generateConeVertices(
					Vector3D(0, 0, 0),
					radius,
					length,
					segments
				)
			),
			radius{ radius }, length{ length }, segments{ segments } {}


		virtual std::vector<Face> calculateFaces(
			const std::vector<Vector3D>& vertices
		) const override {

			std::vector<Face> faces;

			std::vector<std::vector<Vector3D>> vertexFaces =
				returnConeFaces(vertices, segments);

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
				returnConeEdges(vertices, segments);

			for (int i = 0; i < vertexEdges.size(); i++) {
				Edge edge(vertexEdges[i].first, vertexEdges[i].second);
				edges.push_back(edge);
			}

			return edges;
		}


		virtual void setTextureMap() override {}
	};
}


#endif