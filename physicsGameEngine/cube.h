
#ifndef CUBE_H
#define CUBE_H

#include "primitive.h"


namespace pe {

	class Cube : public Primitive {

	public:

		pe::real side;

		Cube(pe::RigidBody* body, const pe::real side, pe::real mass,
			pe::Vector3D position) : Primitive(body, mass, position),
			side{ side } {

			localVertices.resize(8);
			globalVertices.resize(8);

			localVertices[0] = pe::Vector3D(-side / 2, -side / 2, -side / 2);
			localVertices[1] = pe::Vector3D(side / 2, -side / 2, -side / 2);
			localVertices[2] = pe::Vector3D(side / 2, -side / 2, side / 2);
			localVertices[3] = pe::Vector3D(-side / 2, -side / 2, side / 2);
			localVertices[4] = pe::Vector3D(-side / 2, side / 2, -side / 2);
			localVertices[5] = pe::Vector3D(side / 2, side / 2, -side / 2);
			localVertices[6] = pe::Vector3D(side / 2, side / 2, side / 2);
			localVertices[7] = pe::Vector3D(-side / 2, side / 2, side / 2);

			pe::Matrix3x3 inertiaTensor((side* side + side * side),
				0, 0, 0, (side* side + side * side),
				0, 0, 0, (side* side + side * side));
			inertiaTensor *= (mass / 12.0f);
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

		// Connects the correct edges
		virtual void setEdges() override {
			edges.resize(12);

			edges[0].vertices[0] = &globalVertices[0];
			edges[0].vertices[1] = &globalVertices[1];

			edges[1].vertices[0] = &globalVertices[1];
			edges[1].vertices[1] = &globalVertices[2];

			edges[2].vertices[0] = &globalVertices[2];
			edges[2].vertices[1] = &globalVertices[3];

			edges[3].vertices[0] = &globalVertices[3];
			edges[3].vertices[1] = &globalVertices[0];

			edges[4].vertices[0] = &globalVertices[4];
			edges[4].vertices[1] = &globalVertices[5];

			edges[5].vertices[0] = &globalVertices[5];
			edges[5].vertices[1] = &globalVertices[6];

			edges[6].vertices[0] = &globalVertices[6];
			edges[6].vertices[1] = &globalVertices[7];

			edges[7].vertices[0] = &globalVertices[7];
			edges[7].vertices[1] = &globalVertices[4];

			edges[8].vertices[0] = &globalVertices[0];
			edges[8].vertices[1] = &globalVertices[4];

			edges[9].vertices[0] = &globalVertices[1];
			edges[9].vertices[1] = &globalVertices[5];

			edges[10].vertices[0] = &globalVertices[2];
			edges[10].vertices[1] = &globalVertices[6];

			edges[11].vertices[0] = &globalVertices[3];
			edges[11].vertices[1] = &globalVertices[7];
		}

		virtual void setFaces() override {
			faces.resize(6);

			// Defines the vertices for each face in a consistent clockwise order
			faces[0].vertices = { &globalVertices[0], &globalVertices[1],
				&globalVertices[2], &globalVertices[3] }; // Bottom face
			faces[1].vertices = { &globalVertices[7], &globalVertices[6],
				&globalVertices[5], &globalVertices[4] }; // Top face
			faces[2].vertices = { &globalVertices[0], &globalVertices[3],
				&globalVertices[7], &globalVertices[4] }; // Front face
			faces[3].vertices = { &globalVertices[1], &globalVertices[5],
				&globalVertices[6], &globalVertices[2] }; // Back face
			faces[4].vertices = { &globalVertices[0], &globalVertices[4],
				&globalVertices[5], &globalVertices[1] }; // Left face
			faces[5].vertices = { &globalVertices[3], &globalVertices[2],
				&globalVertices[6], &globalVertices[7] }; // Right face
		}

		// Connects the correct edges using local coordinates
		virtual void setLocalEdges() override {
			localEdges.resize(12);

			localEdges[0].vertices[0] = &localVertices[0];
			localEdges[0].vertices[1] = &localVertices[1];

			localEdges[1].vertices[0] = &localVertices[1];
			localEdges[1].vertices[1] = &localVertices[2];

			localEdges[2].vertices[0] = &localVertices[2];
			localEdges[2].vertices[1] = &localVertices[3];

			localEdges[3].vertices[0] = &localVertices[3];
			localEdges[3].vertices[1] = &localVertices[0];

			localEdges[4].vertices[0] = &localVertices[4];
			localEdges[4].vertices[1] = &localVertices[5];

			localEdges[5].vertices[0] = &localVertices[5];
			localEdges[5].vertices[1] = &localVertices[6];

			localEdges[6].vertices[0] = &localVertices[6];
			localEdges[6].vertices[1] = &localVertices[7];

			localEdges[7].vertices[0] = &localVertices[7];
			localEdges[7].vertices[1] = &localVertices[4];

			localEdges[8].vertices[0] = &localVertices[0];
			localEdges[8].vertices[1] = &localVertices[4];

			localEdges[9].vertices[0] = &localVertices[1];
			localEdges[9].vertices[1] = &localVertices[5];

			localEdges[10].vertices[0] = &localVertices[2];
			localEdges[10].vertices[1] = &localVertices[6];

			localEdges[11].vertices[0] = &localVertices[3];
			localEdges[11].vertices[1] = &localVertices[7];
		}

		virtual void setLocalFaces() override {
			localFaces.resize(6);

			localFaces[0].vertices = { &localVertices[0], &localVertices[1],
				&localVertices[2], &localVertices[3] }; // Bottom face

			localFaces[1].vertices = { &localVertices[7], &localVertices[6],
				&localVertices[5], &localVertices[4] }; // Top face

			localFaces[2].vertices = { &localVertices[0], &localVertices[3],
				&localVertices[7], &localVertices[4] }; // Front face

			localFaces[3].vertices = { &localVertices[1], &localVertices[5],
				&localVertices[6], &localVertices[2] }; // Back face

			localFaces[4].vertices = { &localVertices[0], &localVertices[4],
				&localVertices[5], &localVertices[1] }; // Left face

			localFaces[5].vertices = { &localVertices[3], &localVertices[2],
				&localVertices[6], &localVertices[7] }; // Right face
		}

	};
}


#endif