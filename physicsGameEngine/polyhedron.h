/*
	Header file for the general polyhedron class.
*/

#ifndef POLYHEDRON_H
#define POLYHEDRON_H

#include "vector2D.h"
#include "rigidBody.h"
#include "boundingSphere.h"
#include <vector>
#include <algorithm>

#include "face.h"
#include "curvedFace.h"
#include "edge.h"

namespace pe {

	class Polyhedron {

	private:

		/*
			For any shape, there will be a smallest collision box that can
			contain the entire shape. This shape may not be centred at the
			centre of gravity however
			(the geometric centre may not be the centre of gravity,
			which will be closer to areas of the shape with more vertices.
			This is why we need an offset.
			We could instead find the maximum x, y, z, of any vertices,
			and use that as the halfsize from the centre of gravity,
			but then it wouldn't be the smallest collision box anymore.
			(the centre of gravity in a pyramid for example, is closer to
			the base; but the collision box must contain all vertices;
			so if the halfsize were to be calculated from the centre of
			gravity, the box would not be minimal. There would be a large
			gap below the base of the pyramid).
		*/
		static void calculateBoundingBox(
			const std::vector<Vector3D>& vertices,
			Vector3D& halfsize,
			Vector3D& offset
		) {
			if (vertices.empty()) {
				return;
			}

			// Initialize min and max coordinates with the first vertex
			Vector3D minCoord = vertices[0];
			Vector3D maxCoord = vertices[0];

			// Iterate through all vertices to find the minimum and maximum coordinates
			for (const Vector3D& vertex : vertices) {
				minCoord.x = std::min(minCoord.x, vertex.x);
				minCoord.y = std::min(minCoord.y, vertex.y);
				minCoord.z = std::min(minCoord.z, vertex.z);

				maxCoord.x = std::max(maxCoord.x, vertex.x);
				maxCoord.y = std::max(maxCoord.y, vertex.y);
				maxCoord.z = std::max(maxCoord.z, vertex.z);
			}

			// Calculate halfsize by taking half of the distance between min and max coordinates
			halfsize = (maxCoord - minCoord) * 0.5;

			/*
				The bounding box centre may not be the same as the centre of gravity
				of the shape; so we need an offset to the centre of the box.
				The offset will be equal to offset - centre of gravity = offset - 0
				as the centre of gravity is 0 in local coordinates.
			*/
			offset = (maxCoord + minCoord) * 0.5;
		}

	public:

		std::vector<Vector3D> localVertices;
		RigidBody* body;

		std::vector<Face*> faces;
		std::vector<Edge*> edges;
		std::vector<Vector3D> globalVertices;

		Vector3D furthestPoint;

		// Bounding box information
		Vector3D halfsize;
		Vector3D offset;

		Polyhedron(
			real mass,
			const Vector3D& position,
			const Matrix3x3& inertiaTensor,
			const std::vector<Vector3D>& localVertices,
			RigidBody* body
		) : body{ body },
			localVertices{ localVertices },
			furthestPoint(findFurthestPoint())
		{
			body->setMass(mass);
			body->position = position;
			body->setInertiaTensor(inertiaTensor);

			globalVertices = localVertices;

			body->angularDamping = 1;
			body->linearDamping = 1;
			body->calculateDerivedData();

			calculateBoundingBox(localVertices, halfsize, offset);
		}


		void update() {
			globalVertices.clear();
			for (const Vector3D& vertex : localVertices) {
				globalVertices.push_back(
					body->transformMatrix.transform(vertex)
				);
			}

			/*
				Because we use pointers to the vectors of vertices in the
				Polyhedron class in the Faces and Edges, we don't need
				to update them. However, faces also have normals and
				centroids which need to be updated here.
			*/
			for (Face* face : faces) {
				face->update(body->transformMatrix);
			}
		}


		Vector3D findFurthestPoint() const {

			Vector3D furthestPoint = *std::max_element(
				localVertices.begin(),
				localVertices.end(),
				[](const Vector3D& first, const Vector3D& second) -> bool {
					return first.magnitudeSquared()
						< second.magnitudeSquared();
				}
			);
			return furthestPoint;
		}

		
		~Polyhedron() {
			delete body;
			for (int i = 0; i < faces.size(); i++) {
				delete faces[i];
			}
			for (int i = 0; i < edges.size(); i++) {
				delete edges[i];
			}
		}
		

		/*
			In most well known polyhedrons, such as rectangular prisms,
			spheres, and pyramids, the centre of gravity is well known,
			and can be set explicitely using the dimensions of the shape,
			if it is known.
			However, if it is not know (if the shape is not well known),
			we can use this static function to set the centre of gravity
			of the shape (around which, the rotations and relative positions
			are set). This is distinct from the geometric centre, which is
			the centre of the box that contains the shape.
		*/
		static Vector3D calculateCentreOfGravity(
			const std::vector<Vector3D>& vertices
		) {

		}


		/*
			One design issue is wether we create setters for the face
			and edge associations, or we create pure virtual functions
			that need to be implemented in subclasses which force the
			edges and faces to be set.
			We go with the former approach as it provides the flexibility
			of instantiating a polyhedron object without having to have
			a suitable subclass.
		*/
		void setFaces(std::vector<Face*> faces) {
			this->faces = faces;
		}

		/*
			We can usually get the edges from the faces, without having
			to define them explicitely; however this creates duplicated
			edges, which slow the engine down, both in the graphics
			module and the collision detection system.
			So for simple shapes, where we can define unique edges,
			we use this function.
		*/
		void setEdges(std::vector<Edge*> edges) {
			this->edges = edges;
		}


		Vector3D getAxis(int index) const {
			return body->transformMatrix.getColumnVector(index);
		}


		Vector3D getCentre() const {
			return body->position;
		}


		Vector3D getFaceNormal(int index) const {
			return faces[index]->getNormal();
		}


		/*
			Function used in the GJK collision detection and generation
			algorithm; it finds the furthest point from a given direction
			in the Polyhedron.
		*/
		Vector3D support(const Vector3D& direction) const {
			// Initialize with the first vertex
			Vector3D furthestVertex = globalVertices[0];
			real maxDot = furthestVertex.scalarProduct(direction);

			// Iterate through all vertices to find the one farthest in the given direction
			for (size_t i = 1; i < globalVertices.size(); ++i) {
				real dotProduct = globalVertices[i].scalarProduct(direction);
				if (dotProduct > maxDot) {
					maxDot = dotProduct;
					furthestVertex = globalVertices[i];
				}
			}

			return furthestVertex;
		}


		/*
			The halfsize of the shape can be used to determine its collision
			box (the rectangular prism which encompasses it) in case we want
			to avoid doing detailed collision detection for complex shapes.
			The same applies for collision spheres, which will have
			the radius as the magnitude of the halfsize.
		*/
		Vector3D getHalfsize() const {
			return halfsize;
		}


		// Returns the offset of the collision box
		Vector3D getOffset() const {
			return offset;
		}

		Vector3D getFurthestPoint() const {
			return furthestPoint;
		}
	};
}

#endif