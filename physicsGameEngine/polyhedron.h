
#ifndef POLYHEDRON_H
#define POLYHEDRON_H

#include "rigidBody.h"
#include <vector>

namespace pe {

	/*
		A struct representing a polyhedron face, which includes a vector
		of vertices, its normal, and centroid (which are calculated once
		at the moment of creation).
		The vertices must always be in order, both for the collision
		detection algorithms and the graphics to work correctly. Moreover,
		they must be ordered consistentely, either always counter-clockwise
		or clockwise.
		In this engine, we always use counter-clockwise (positive
		direction).

		We have a choice to make here:
			- Either we can create an update function to reset the vertices
			after applying a transformation, and then call calculateNormal
			and calculateCentroid again.
			- We can do the same as above, but use the transformation also
			on the normal and centroid, instead of recalculating it.
			- Or we can scrap the object and create a new face each time we
			update the Polyhedron.

		Recalculating the normal and centroid is cheaper than using the
		transform matrix. Moreover, scrapping the object and recreating a
		new one is simpler, and involves less of a hassle with no
		performance hit. So we got with the third option: once the object is
		created, it can't be changed.
	*/
	struct Face {
		std::vector<Vector3D> vertices;
		Vector3D normal;
		Vector3D centroid;

		Face() {}

		Face(const std::vector<Vector3D>& vertices) : vertices(vertices) {
			calculateNormal();
			calculateCentroid();
		}


		// This function assumes we have at least 3 vertices
		void calculateNormal() {
			/*
				Sometimes, we get degenerate vertices, where some vertices
				in a face aren't all distinct. This makes calculating the
				normal difficult. this function assumes that there are at
				least 3 distinct vertices, finds the first 3, and then
				calculates the normal.
			*/

			Vector3D firstVertex = vertices[0];
			Vector3D secondVertex;
			for (int i = 1; i < vertices.size(); i++) {
				if (vertices[i] != firstVertex) {
					secondVertex = vertices[i];
					break;
				}
			}
			Vector3D thirdVertex;
			for (int i = 1; i < vertices.size(); i++) {
				if (vertices[i] != firstVertex
					&& vertices[i] != secondVertex) {
					thirdVertex = vertices[i];
					break;
				}
			}

			Vector3D AB = secondVertex - firstVertex;
			Vector3D AC = thirdVertex - firstVertex;;
			normal = AB.vectorProduct(AC);
			normal.normalize();
		}

		void calculateCentroid() {
			Vector3D sum;
			// Calculate the sum of vertex positions
			for (const Vector3D vertex : vertices) {
				sum += vertex;
			}
			// Calculate the centroid as the average of vertex positions
			centroid = sum * (1.0f / static_cast<real>(vertices.size()));
		}
	};


	/*
		A struct representing the edge of a polyhedron.
	*/
	struct Edge {
		std::pair<Vector3D, Vector3D> vertices;

		Edge() {}

		Edge(const Vector3D& first, const Vector3D& second) {
			vertices = std::make_pair(first, second);
		}
	};


	class Polyhedron {

	public:

		// The vertices in local coordinates
		std::vector<Vector3D> localVertices;
		RigidBody* body;

		/*
			The Faces and Edges, in global coordinates.
			While its deifnitely very important to have the local vertices
			of the polyhedron in local coordinates as a reference point to
			go back to (so that we can always get the coordinates position
			by applying a transformation on them), having the faces and
			edges be in global coordinates offers its own set of benefits.
			For instance, the faces and edges will be used in several
			modules, including the graphics and collision modules.
			Instead of each time transforming the local vertices and
			reconnecting them using the setEdges and setFaces functions
			(which know how to, based on indeces of the local, and by
			extension, global array, associate the vertices.
		*/
		std::vector<Face> faces;
		std::vector<Edge> edges;
		std::vector<Vector3D> globalVertices;


		Polyhedron(
			RigidBody* body,
			real mass,
			const Vector3D& position,
			const Matrix3x3& inertiaTensor,
			const std::vector<Vector3D>& localVertices) : body{ body } {
			body->setMass(mass);
			body->position = position;
			body->setInertiaTensor(inertiaTensor);
			this->localVertices = localVertices;
			// Initially, the global vertices are the same as the local ones
			globalVertices = localVertices;

			body->angularDamping = 1;
			body->linearDamping = 1;
			body->calculateDerivedData();
		}


		/*
			Returns the edges associations of the given vertices as defined
			in the class that extends Polyhedron.
			Needs to be overriden in such a way as to return an array of
			Edge objects based on which vertices the edges connect in the
			shape.
			Designed to provide a way to get the edges of the current
			polyhedron regardless of what the value of the actual vertices
			are (wether they're local or transformed), so long as they are
			of the expected order of the local vertices as defined by the
			class extending Polyhedron and overriding this function.
		*/
		virtual std::vector<Edge> calculateEdges(
			const std::vector<Vector3D>& vertices
		) const = 0;


		/*
			Returns the faces associations.
			Everything said about the edges applies here.
		*/
		virtual std::vector<Face> calculateFaces(
			const std::vector<Vector3D>& vertices
		) const = 0;


		/*
			Updates the faces and edges by the body's transform matrix.
		*/
		void update() {
			globalVertices.clear();
			for (const Vector3D& vertex : localVertices) {
				globalVertices.push_back(
					body->transformMatrix.transform(vertex)
				);
			}

			edges = calculateEdges(globalVertices);
			faces = calculateFaces(globalVertices);
		}
	};
}

#endif