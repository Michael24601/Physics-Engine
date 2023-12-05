
#ifndef PYRAMID_H
#define PYRAMID_H

#include "polyhedron.h"

namespace pe {

	class Pyramid : public Polyhedron {

	public:

		// Side length
		real side;
		real height;

		Pyramid(
			RigidBody* body, 
			real side, 
			real height, 
			real mass,
			Vector3D position) : 
			Polyhedron(
				body,
				mass,
				position,
				Matrix3x3(
					(mass / 10.0)* (3 * side * side + height * height), 0, 0,
					0, (mass / 10.0)* (3 * side * side + height * height), 0,
					0, 0, (mass / 5.0)* side* side
				),
				std::vector<Vector3D>{
					Vector3D(0, 3 * height / 4.0, 0),
					Vector3D(-side / 2, -height / 4.0, -side / 2),
					Vector3D(side / 2, -height / 4.0, -side / 2),
					Vector3D(side / 2, -height / 4.0, side / 2),
					Vector3D(-side / 2, -height / 4.0, side / 2),
				}
			),
			side{ side }, height{ height } {}


		virtual std::vector<Edge> calculateEdges(
			const std::vector<Vector3D>& vertices
		) const override {

			std::vector<Edge> edges;
			edges.resize(8);

			// Define the edges of the pyramid
			edges[0] = Edge(vertices[1], vertices[2]);
			edges[1] = Edge(vertices[2], vertices[3]);
			edges[2] = Edge(vertices[3], vertices[4]);
			edges[3] = Edge(vertices[4], vertices[1]);
			edges[4] = Edge(vertices[0], vertices[1]);
			edges[5] = Edge(vertices[0], vertices[2]);
			edges[6] = Edge(vertices[0], vertices[3]);
			edges[7] = Edge(vertices[0], vertices[4]);

			return edges;
		}

		// All vertices are in clockwise order
		virtual std::vector<Face> calculateFaces(
			const std::vector<Vector3D>& vertices
		) const override {

			std::vector<Face> faces;
			faces.resize(5);

			// Base face
			faces[0] = Face(std::vector<Vector3D>{ vertices[1], vertices[2],
				vertices[3], vertices[4] });

			// Side faces
			faces[1] = Face(std::vector<Vector3D>{ vertices[0], vertices[1],
				vertices[4] });
			faces[2] = Face(std::vector<Vector3D>{ vertices[0], vertices[4],
				vertices[3] });
			faces[3] = Face(std::vector<Vector3D>{ vertices[0], vertices[3],
				vertices[2] });
			faces[4] = Face(std::vector<Vector3D>{ vertices[0], vertices[2],
				vertices[1] });

			return faces;
		}
	};
}

#endif

