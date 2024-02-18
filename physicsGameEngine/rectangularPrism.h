
#ifndef RECTANGULAR_PRISM_H
#define RECTANGULAR_PRISM_H

#include "polyhedron.h"

namespace pe {

	class RectangularPrism : public Polyhedron {

	public:

		real width;
		real height;
		real depth;

		RectangularPrism(
			RigidBody* body,
			real width,
			real height,
			real depth,
			real mass,
			Vector3D position) :
			Polyhedron(
				body,
				mass,
				position,
				Matrix3x3(
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
			width{ width }, height{ height }, depth{ depth } {

			setEdges();
			setFaces();
		}


		virtual void setEdges() override {
			edges.resize(12);

			// Define the edges of the pyramid
			edges[0] = new Edge(&localVertices, &globalVertices, 0, 1);
			edges[1] = new Edge(&localVertices, &globalVertices, 1, 2);
			edges[2] = new Edge(&localVertices, &globalVertices, 2, 3);
			edges[3] = new Edge(&localVertices, &globalVertices, 3, 0);
			edges[4] = new Edge(&localVertices, &globalVertices, 4, 5);
			edges[5] = new Edge(&localVertices, &globalVertices, 5, 6);
			edges[6] = new Edge(&localVertices, &globalVertices, 6, 7);
			edges[7] = new Edge(&localVertices, &globalVertices, 7, 4);
			edges[8] = new Edge(&localVertices, &globalVertices, 0, 4);
			edges[9] = new Edge(&localVertices, &globalVertices, 1, 5);
			edges[10] = new Edge(&localVertices, &globalVertices, 2, 6);
			edges[11] = new Edge(&localVertices, &globalVertices, 3, 7);
		}


		// All vertices are in clockwise order
		virtual void setFaces() override {
			faces.resize(6);

			std::vector<std::vector<int>> indexes {
				std::vector<int>{ 0, 1, 2, 3 },
				std::vector<int>{ 7, 6, 5, 4 },
				std::vector<int>{ 0, 3, 7, 4 },
				std::vector<int>{ 1, 5, 6, 2 },
				std::vector<int>{ 0, 4, 5, 1 },
				std::vector<int>{ 3, 2, 6, 7 }
			};

			for (int i = 0; i < indexes.size(); i++) {
				faces[i] = new Face(
					&localVertices, 
					&globalVertices, 
					indexes[i]
				);
			}
		}
	};
}


#endif