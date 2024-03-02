

#ifndef CONE_H
#define CONE_H

#include "polyhedron.h"

namespace pe {

	class Cone : public Polyhedron {

	private:

		/*
			Defines the vertices of a tessalated cone with a certain radius,
			height, and a number of segments (the more segments the more
			it will look like a cone). 
		*/
		static std::vector<Vector3D> generateConeVertices(
			const Vector3D& center,
			real radius, 
			real height, 
			int segments
		) {

			std::vector<Vector3D> vertices;

			// Adds the apex of the cone at the top
			vertices.push_back(center + Vector3D(0.0f, height / 2.0f, 0.0f));

			for (int i = 0; i <= segments; ++i) {
				// Calculate the angle for each segment
				real theta = 2.0f * PI * static_cast<real>(i) / 
					static_cast<real>(segments);

				// Vertices of the current point on the base of the cone
				Vector3D v_base = center + Vector3D(
					radius * cos(theta),
					-height / 2.0f,
					radius * sin(theta)
				);

				vertices.push_back(v_base);
			}

			return vertices;
		}

	public:

		real radius;
		real length;
		int segments;

		Cone(
			real radius,
			real length,
			real mass,
			int segments,
			Vector3D position) :
			Polyhedron(
				mass,
				position,
				Matrix3x3(
					(3.0 / 80.0)* mass* (radius* radius + 4.0 * length * length), 0, 0,
					0, (3.0 / 80.0)* mass* (radius* radius + 4.0 * length * length), 0,
					0, 0, (3.0 / 40.0)* mass* radius* radius
				),
				generateConeVertices(
					Vector3D(0, 0, 0),
					radius,
					length,
					segments
				)
			),
			radius{ radius }, length{ length }, segments{ segments } {

			setEdges();
			setFaces();
		}


		virtual void setFaces() override {

			// Base face
			std::vector<int> baseFaceIndexes;
			for (int i = 1; i <= segments; i++) {
				baseFaceIndexes.push_back(i);
			}
			Face* baseFace = new Face(
				&localVertices, 
				&globalVertices, 
				baseFaceIndexes
			);
			faces.push_back(baseFace);

			// Side faces
			for (int i = 1; i <= segments; ++i) {
				int v0 = i;
				int v1 = (i % segments) + 1;

				std::vector<int> sideFaceIndexes = {
					0, // Apex
					v1,
					v0
				};

				/*
					Note that all the side faces are curved, unlike the base.
					The normals of the two bottom vertices can be calculated
					as the normal vector from the centroid of the base to
					each of the vertices, and the normal at the apex of the
					side face is the one between the two.

					The normals are sent to the face in the same order as
					the vertex indexes: counter-clockwise.
				*/
				Vector3D normalBase0 = localVertices[v0] - baseFace->getCentroid();
				Vector3D normalBase1 = localVertices[v1] - baseFace->getCentroid();
				Vector3D normalApex = normalBase0 + normalBase1;

				std::vector<Vector3D> normals{
					normalApex.normalized(),
					normalBase1.normalized(),
					normalBase0.normalized()
				};

				CurvedFace* sideFace = new CurvedFace(
					&localVertices,
					&globalVertices,
					sideFaceIndexes,
					normals
				);
				faces.push_back(sideFace);
			}
		}

		virtual void setEdges() override {

			// Base edges
			for (int i = 1; i <= segments; ++i) {
				// Indexes of the two vertices of the current edge
				int v0 = i;
				int v1 = (i % segments) + 1;

				// Connects vertices to form edges based on the side faces
				Edge* edge = new Edge(&localVertices, &globalVertices, v0, v1);
				edges.push_back(edge);
			}

			// Edges from the apex to the base vertices
			for (int i = 1; i <= segments; ++i) {
				Edge* edge = new Edge(&localVertices, &globalVertices, 0, i);
				edges.push_back(edge);
			}
		}

	};
}


#endif