
#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include <iostream>
#include "rigidBody.h"
#include "drawingUtil.h"

namespace pe {

	// Assumes the face has at least 3 vertices
	struct Face {
		std::vector<Vector3D*> vertices;

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
		Vector3D normal() const {
			// Calculate the local normal using cross product
			Vector3D AB = *vertices[1] - *vertices[0];
			Vector3D AC = *vertices[2] - *vertices[0];
			Vector3D normal = AB.vectorProduct(AC);
			normal.normalize();
			return normal;
		}

		/*
			Function to calculate the centroid of the face.
			Calculated each frame from the global coordinates of the vertices.
			Another approach is to calculate it once at the start and use
			a transform matrix to get its new position.
		*/
		Vector3D centroid() const {
			Vector3D sum(0, 0, 0);

			// Calculate the sum of vertex positions
			for (const Vector3D* vertex : vertices) {
				sum += *vertex;
			}

			// Calculate the centroid as the average of vertex positions
			return sum * (1.0f / static_cast<real>(vertices.size()));
		}
	};

	struct Edge {
		// Should have 2 elements
		std::vector<Vector3D*> vertices;
		Edge() : vertices(2) {}
	};

	/*
		Class used for a convex 3D shape
	*/
	class Primitive {

	public:

		// Body of the primitive
		RigidBody* body;

		// Local vertices
		std::vector<Vector3D> localVertices;

		// Global vertices - updated each frame
		std::vector<Vector3D> globalVertices;

		// Faces and edges in global variables
		std::vector<Face> faces;
		std::vector<Edge> edges;

		// Constructor, only takes body at this stage
		Primitive(RigidBody* body, real mass,
			const Vector3D& position) : body{ body } {
			body->setMass(mass);
			body->position = position;
		}

		/*
			Function that returns the normals of each face using the built in
			face normal function, which means they don't need to be 
			transformed and are calculates each frame in this function.
		*/
		std::vector<std::pair<Vector3D, Vector3D>> 
			calculateFaceNormals(real length) const;


		// Returns the edges of the shape (used to draw it, or for debugging)
		std::vector<std::pair<Vector3D, Vector3D>> getEdges() const;

		/*
			Updates the global variables using the transform matrix.
			Since the faces and edges use pointers, they don't need
			to be updated. The normal of the face is returned from
			a function so it also need not be updated.
		*/
		void updateVertices();

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
		bool isColliding(const Primitive& primitive) const;
	};
}

#endif