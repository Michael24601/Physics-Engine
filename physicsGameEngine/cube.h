
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
		}

		// Connects the correct edges
		virtual void setEdges() override {
			edges.resize(12);

			edges[0] = { &globalVertices[0], &globalVertices[1] };
			edges[1] = { &globalVertices[1], &globalVertices[2] };
			edges[2] = { &globalVertices[2], &globalVertices[3] };
			edges[3] = { &globalVertices[3], &globalVertices[0] };
			edges[4] = { &globalVertices[4], &globalVertices[5] };
			edges[5] = { &globalVertices[5], &globalVertices[6] };
			edges[6] = { &globalVertices[6], &globalVertices[7] };
			edges[7] = { &globalVertices[7], &globalVertices[4] };
			edges[8] = { &globalVertices[0], &globalVertices[4] };
			edges[9] = { &globalVertices[1], &globalVertices[5] };
			edges[10] = { &globalVertices[2], &globalVertices[6] };
			edges[11] = { &globalVertices[3], &globalVertices[7] };
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
	};
}