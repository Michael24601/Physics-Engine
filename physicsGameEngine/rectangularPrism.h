
#ifndef RECTANGULAR_PRISM_H
#define RECTANGULAR_PRISM_H

#include "polyhedron.h"

namespace pe {

	class RectangularPrism : public Polyhedron {

	public:

		real width;
		real height;
		real depth;

		RectangularPrism(RigidBody* body, real width, real height,
			real depth, real mass, Vector3D position) :
			Polyhedron(
				body,
				mass,
				position,
				Matrix3x3 (
					(mass / 12.0)* (height* height + depth * depth), 0, 0,
					0, (mass / 12.0)* (width* width + depth * depth), 0,
					0, 0, (mass / 12.0)* (width* width + height * height)
				),
				std::vector<Vector3D>{
					Vector3D(-width / 2, -height / 2, -depth / 2),
					Vector3D(width / 2, -height / 2, -depth / 2),
					Vector3D(width / 2, -height / 2, depth / 2),
					Vector3D(-width / 2, -height / 2, depth / 2),
					Vector3D(-width / 2, height / 2, -depth / 2),
					Vector3D(width / 2, height / 2, -depth / 2),
					Vector3D(width / 2, height / 2, depth / 2),
					Vector3D(-width / 2, height / 2, depth / 2)
				}
			),
			width{ width }, height{ height }, depth{ depth } {}


		virtual std::vector<Edge> calculateEdges(
			const std::vector<Vector3D>& vertices
		) const override {

			std::vector<Edge> edges;
			edges.resize(12);

			// Define the edges of the pyramid
			edges[0] = Edge(vertices[0], vertices[1]);
			edges[1] = Edge(vertices[1], vertices[2]);
			edges[2] = Edge(vertices[2], vertices[3]);
			edges[3] = Edge(vertices[3], vertices[0]);
			edges[4] = Edge(vertices[4], vertices[5]);
			edges[5] = Edge(vertices[5], vertices[6]);
			edges[6] = Edge(vertices[6], vertices[7]);
			edges[7] = Edge(vertices[7], vertices[4]);
			edges[8] = Edge(vertices[0], vertices[4]);
			edges[9] = Edge(vertices[1], vertices[5]);
			edges[10] = Edge(vertices[2], vertices[6]);
			edges[11] = Edge(vertices[3], vertices[7]);

			return edges;
		}


		// All vertices are in clockwise order
		virtual std::vector<Face> calculateFaces(
			const std::vector<Vector3D>& vertices
		) const override {

			std::vector<Face> faces;
			faces.resize(6);

			faces[0] = Face(std::vector<Vector3D>{ vertices[0], 
				vertices[1], vertices[2], vertices[3] });
			faces[1] = Face(std::vector<Vector3D>{ vertices[7],
				vertices[6], vertices[5], vertices[4] });
			faces[2] = Face(std::vector<Vector3D>{ vertices[0],
				vertices[3], vertices[7], vertices[4] });
			faces[3] = Face(std::vector<Vector3D>{ vertices[1],
				vertices[5], vertices[6], vertices[2]});
			faces[4] = Face(std::vector<Vector3D>{ vertices[0],
				vertices[4], vertices[5], vertices[1] });
			faces[5] = Face(std::vector<Vector3D>{ vertices[3],
				vertices[2], vertices[6], vertices[7] });

			return faces;
		}
	};
}


#endif