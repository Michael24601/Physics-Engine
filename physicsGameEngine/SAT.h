
#ifndef SAT_H
#define SAT_H

#include "rigidBody.h"
#include "SFML/Graphics.hpp"
#include <vector>

namespace sat {

	// Pre-declarations

	class Primitive;
	int whichSide(const Primitive& C, const pe::Vector3D& P,
		const pe::Vector3D& D);
	bool testIntersection(const Primitive& C0, const Primitive& C1);


	// Assumes the face has at least 3 vertices
	struct Face{
		std::vector<pe::Vector3D*> vertices;
		
		/*
			We can either return the normal in a function each time,
			removing the need to update a variable each frame, but slowing
			down the program as it is recalculated each time, or we can
			have the local normal saved in a variable, and each frame, update
			the global normal using the transform matrix of the body.
			We choose the first approach as the normal is only rarely called,
			making it likely faster.
			However, this means that the vertices need to be given in a
			consistent direction, meaning that if the vertices are defined
			clockwise in one face, it needs to be consistent in the others.
			This ensure the outer normal, not inner normal (opposite
			direction), is returned each time.
		*/
		pe::Vector3D normal() const {
			// Calculate the local normal using cross product
			pe::Vector3D AB = *vertices[1] - *vertices[0];
			pe::Vector3D AC = *vertices[2] - *vertices[0];
			pe::Vector3D normal = AB.vectorProduct(AC);
			normal.normalize();
			return normal;
		}
	};

	struct Edge {
		pe::Vector3D* vertices[2];
	};

	/*
		Class used for a convex 3D shape
	*/
	class Primitive {

	public:

		// Body of the primitive
		pe::RigidBody* body;

		// Local vertices
		std::vector<pe::Vector3D> localVertices;

		// Global vertices - updated each frame
		std::vector<pe::Vector3D> globalVertices;

		// Faces and edges in global variables
		std::vector<Face> faces;
		std::vector<Edge> edges;

		// Constructor, only takes body at this stage
		Primitive(pe::RigidBody* body, pe::real mass, 
			const pe::Vector3D& position) : body{ body } {
			body->setMass(mass);
			body->position = position;
		}

		sf::VertexArray drawLine(pe::Vector3D* c1, pe::Vector3D* c2) const {
			sf::VertexArray line(sf::LineStrip, 2);
			line[0].position = sf::Vector2f(c1->x, c1->y);
			line[1].position = sf::Vector2f(c2->x, c2->y);
			line[0].color = line[1].color = sf::Color::White;

			return line;
		}

		// Returns lines to draw, which are the edges
		std::vector<sf::VertexArray> drawLines() const {
			std::vector<sf::VertexArray> lines;
			lines.resize(edges.size());
			for (int i = 0; i < edges.size(); i++) {
				lines[i] = drawLine(edges[i].vertices[0],
					edges[i].vertices[1]);
			}
			return lines;
		}

		/*
			Updates the global variables using the transform matrix.
			Since the faces and edges use pointers, they don't need
			to be updated. The normal of the face is returned from
			a function so it also need not be updated.
			It also updates the normals. 
		*/
		void updateVertices() {
			for (int i = 0; i < globalVertices.size(); i++) {
				globalVertices[i] =
					body->transformMatrix.transform(localVertices[i]);
			}
		}

		// Used to set teh edges and faces when the class is extended
		virtual void setEdges() = 0;

		/*
			Make sure the vertices are always given in the same order
			(clockwise or anti-clockwise in relatiion to the outside or the
			inside of the convex shape). This is necessary for SAT in order
			for the normal to always be on the outside of the face as
			calculated by the Face class.
		*/
		virtual void setFaces() = 0;
		
		// Uses SAT to check if the convex shapes are colliding
		bool isColliding(const Primitive& primitive) {
			return testIntersection(*this, primitive);
		}
	};


	int whichSide(const Primitive& C, const pe::Vector3D& P,
		const pe::Vector3D& D) {
		/*
			The vertices are projected to the form P + t * D. The return value
			is +1 if all t>0, = 1 if all t<0, but 0 otherwise, in which case the
			line splits the polygon projection.
		*/
		int positive = 0, negative = 0;
		for (int i = 0; i < C.globalVertices.size(); ++i) {
			// Project a vertex onto the line.
			pe::real t = D.scalarProduct(C.globalVertices[i] - P);
			if (t > 0) {
				++positive;
			}
			else if (t < 0) {
				++negative;
			}
			if (positive && negative) {
				/*
					The polygon has vertices on both sides of the line, so the
					line is not a separating axis. Time is saved by not having
					to project the remaining vertices.
				*/
				return 0;
			}
		}
		// Either positive > 0 or negative > 0 but not both are positive.
		return (positive > 0 ? +1 : -1);
	}


	bool testIntersection(const Primitive& C0, const Primitive& C1) {
		/*
			Test faces of C0 for separation. Because of the counter clockwise
			ordering, the  projection interval for C0 is [T,0] whereT<0.
			Determine whether C1 is on the  positive side of the line.
		*/
		for (int i = 0; i < C0.faces.size(); ++i) {
			pe::Vector3D P = *C0.faces[i].vertices[0];
			pe::Vector3D N = C0.faces[i].normal(); // outward pointing
			if (whichSide(C1, P, N) > 0) {
				// C1 is entirely on the positive side of the line P + t * N.
				return false;
			}
		}
		/*
			Test faces of C1 for separation. Because of the counterclockwise
			ordering, the  projection interval for C1 is [T,0] whereT<0.
			Determine whether C0 is on the  positive side of the line.
		*/
		for (int i = 0; i < C1.faces.size(); ++i) {
			pe::Vector3D P = *C1.faces[i].vertices[0];
			pe::Vector3D N = C1.faces[i].normal(); // outward pointing
			if (whichSide(C0, P, N) > 0) {
				// C1 is entirely on the positive side of the line P + t * N.
				return false;
			}
		}

		/*
			Test cross products of pairs of edge directions, one edge direction
			from each polyhedron.
		*/
		for (int i0 = 0; i0 < C0.globalVertices.size(); ++i0) {
			pe::Vector3D D0 = *C0.edges[i0].vertices[1] - *C0.edges[i0].vertices[0];
			pe::Vector3D P = *C0.edges[i0].vertices[0];
			for (int i1 = 0; i1 < C1.globalVertices.size(); ++i1) {
				pe::Vector3D D1 = *C1.edges[i1].vertices[1] - *C1.edges[i1].vertices[0];
				pe::Vector3D N = D0.vectorProduct(D1);
				if (N != pe::Vector3D(0, 0, 0)) {
					int side0 = whichSide(C0, P, N);
					if (side0 == 0) {
						continue;
					}
					int side1 = whichSide(C1, P, N);
					if (side1 == 0) {
						continue;
					}
					if (side0 * side1 < 0) {
						/*
							The projections of C0 and C1 onto the line P + t * N are
							on opposite sides of the projection of P.
						*/
						return false;
					}
				}
			}
		}
		return true;
	}

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

			pe::Matrix3x3 inertiaTensor((side * side + side * side),
				0, 0, 0, (side * side + side * side),
				0, 0, 0, (side * side + side * side));
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
		}

		virtual void setEdges() override {
			edges.resize(8);

			// Define the edges of the pyramid
			edges[0] = { &globalVertices[0], &globalVertices[1] };
			edges[1] = { &globalVertices[1], &globalVertices[2] };
			edges[2] = { &globalVertices[2], &globalVertices[3] };
			edges[3] = { &globalVertices[3], &globalVertices[4] };
			edges[4] = { &globalVertices[4], &globalVertices[1] };
			edges[5] = { &globalVertices[0], &globalVertices[2] };
			edges[6] = { &globalVertices[0], &globalVertices[3] };
			edges[7] = { &globalVertices[0], &globalVertices[4] };
		}

		// All vertices are in clockwise order
		virtual void setFaces() override {
			faces.resize(5); // Pyramid has 5 faces

			// Base face
			faces[0].vertices = { &globalVertices[1], &globalVertices[2],
				&globalVertices[3], &globalVertices[4] };

			// Side faces
			faces[1].vertices = { &globalVertices[0], &globalVertices[1],
				&globalVertices[4] };
			faces[2].vertices = { &globalVertices[0], &globalVertices[4],
				&globalVertices[3] };
			faces[3].vertices = { &globalVertices[0], &globalVertices[3],
				&globalVertices[2] };
			faces[4].vertices = { &globalVertices[0], &globalVertices[2],
				&globalVertices[1] };
		}
	};
}

#endif