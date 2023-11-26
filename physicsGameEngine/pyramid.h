
#ifndef PYRAMID_H
#define PYRAMID_H

#include "primitive.h"

namespace pe {

	class Pyramid : public Primitive {

	public:

		// Side length
		real side;
		real height;

		Pyramid(pe::RigidBody* body, real side, real height, real mass,
			Vector3D position) : Primitive(body, mass, position),
			side{ side }, height{ height } {
			localVertices.resize(5);
			globalVertices.resize(5);
			localVertices[0] = Vector3D(0, -3 * height / 4.0, 0);
			localVertices[1] = Vector3D(-side / 2, height / 4.0, -side / 2);
			localVertices[2] = Vector3D(side / 2, height / 4.0, -side / 2);
			localVertices[3] = Vector3D(side / 2, height / 4.0, side / 2);
			localVertices[4] = Vector3D(-side / 2, height / 4.0, side / 2);

			Matrix3x3 inertiaTensor(
				mass* (3 * height * height + side * side) / 12.0f,
				0, 0, 0, mass* (3 * height * height + side * side) / 12.0f,
				0, 0, 0, (mass* side* side) / 6.0f);
			body->setInertiaTensor(inertiaTensor);

			body->angularDamping = 1;
			body->linearDamping = 1;

			body->calculateDerivedData();
			updateVertices();

			// Sets the faces and edges connections
			
			setEdges();
			setFaces(); 
			setLocalEdges();
			setLocalFaces();
		}

		virtual void setEdges() override {
			edges.resize(8);

			// Define the edges of the pyramid
			edges[0].vertices[0] = &globalVertices[1];
			edges[0].vertices[1] = &globalVertices[2];

			edges[1].vertices[0] = &globalVertices[2];
			edges[1].vertices[1] = &globalVertices[3];

			edges[2].vertices[0] = &globalVertices[3];
			edges[2].vertices[1] = &globalVertices[4];

			edges[3].vertices[0] = &globalVertices[4];
			edges[3].vertices[1] = &globalVertices[1];

			edges[4].vertices[0] = &globalVertices[0];
			edges[4].vertices[1] = &globalVertices[1];

			edges[5].vertices[0] = &globalVertices[0];
			edges[5].vertices[1] = &globalVertices[2];

			edges[6].vertices[0] = &globalVertices[0];
			edges[6].vertices[1] = &globalVertices[3];

			edges[7].vertices[0] = &globalVertices[0];
			edges[7].vertices[1] = &globalVertices[4];
		}

		// All vertices are in clockwise order
		virtual void setFaces() override {
			faces.resize(5); // Pyramid has 5 faces

			// Base face
			faces[0].vertices = { &globalVertices[1], &globalVertices[4],
				&globalVertices[3], &globalVertices[2] };

			// Side faces
			faces[1].vertices = { &globalVertices[0], &globalVertices[4],
				&globalVertices[1] };
			faces[2].vertices = { &globalVertices[0], &globalVertices[1],
				&globalVertices[2] };
			faces[3].vertices = { &globalVertices[0], &globalVertices[2],
				&globalVertices[3] };
			faces[4].vertices = { &globalVertices[0], &globalVertices[3],
				&globalVertices[4] };
		}



		// Update setEdges to populate localEdges using local vertices
		virtual void setLocalEdges() override {
			localEdges.resize(8);

			// Define the edges of the pyramid using local vertices
			localEdges[0].vertices[0] = &localVertices[1];
			localEdges[0].vertices[1] = &localVertices[2];

			localEdges[1].vertices[0] = &localVertices[2];
			localEdges[1].vertices[1] = &localVertices[3];

			localEdges[2].vertices[0] = &localVertices[3];
			localEdges[2].vertices[1] = &localVertices[4];

			localEdges[3].vertices[0] = &localVertices[4];
			localEdges[3].vertices[1] = &localVertices[1];

			localEdges[4].vertices[0] = &localVertices[0];
			localEdges[4].vertices[1] = &localVertices[1];

			localEdges[5].vertices[0] = &localVertices[0];
			localEdges[5].vertices[1] = &localVertices[2];

			localEdges[6].vertices[0] = &localVertices[0];
			localEdges[6].vertices[1] = &localVertices[3];

			localEdges[7].vertices[0] = &localVertices[0];
			localEdges[7].vertices[1] = &localVertices[4];
		}

		// Update setFaces to populate localFaces using local vertices
		virtual void setLocalFaces() override {
			localFaces.resize(5); // Pyramid has 5 faces

			// Base face using local vertices
			localFaces[0].vertices = { &localVertices[1], &localVertices[4],
				&localVertices[3], &localVertices[2] };
			// Side faces using local vertices
			localFaces[1].vertices = { &localVertices[0], &localVertices[4], &localVertices[1] };
			localFaces[2].vertices = { &localVertices[0], &localVertices[1], &localVertices[2] };
			localFaces[3].vertices = { &localVertices[0], &localVertices[2], &localVertices[3] };
			localFaces[4].vertices = { &localVertices[0], &localVertices[3], &localVertices[4] };
		}
	};
}

#endif

