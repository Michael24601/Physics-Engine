
#ifndef CYLINDER_H
#define CYLINDER_H

#include "polyhedron_2.h"

namespace pe {

	class Cylinder : public Polyhedron {

	private:

		static std::vector<Vector3D> generateCylinderVertices(
			const Vector3D& center,
			real radius, 
			real height, 
			int segments
		) {
			std::vector<Vector3D> vertices;

			for (int i = 0; i <= segments; ++i) {
				// Calculate the angle for each segment
				real theta = 2.0f * PI * static_cast<real>(i) / 
					static_cast<real>(segments);

				// Vertices of the current point on the side of the cylinder
				Vector3D v_side;
				v_side += Vector3D(
					radius * cos(theta),
					height / 2.0f,
					radius * sin(theta)
				);
				v_side += center;

				/*
					Vertices of the corresponding point at the bottom of 
					the cylinder.
				*/
				Vector3D v_bottom = v_side - Vector3D(0.0f, height, 0.0f);

				vertices.push_back(v_side);
				vertices.push_back(v_bottom);
			}

			return vertices;
		}

	public:

		real radius;
		real length;
		int segments;

		Cylinder(
			RigidBody* body,
			real radius,
			real length,
			real mass,
			int segments,
			Vector3D position) :
			Polyhedron(
				body,
				mass,
				position,
				Matrix3x3(
					(1.0 / 12.0)* mass*
					(3.0 * radius * radius + length * length), 0, 0,
					0, (1.0 / 12.0)* mass*
					(3.0 * radius * radius + length * length), 0,
					0, 0, (1.0 / 12.0)* mass* (3.0 * radius * radius)
				),
				generateCylinderVertices(
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
			// Top face, in counter-clockwise order
			std::vector<int> topFaceIndexes;
			for (int i = segments - 1; i >= 0; i--) {
				int v0 = 2 * i;
				topFaceIndexes.push_back(v0);
			}
			faces.push_back(
				Face(&localVertices, &globalVertices, topFaceIndexes)
			);

			// Bottom face, in counter-clockwise order
			std::vector<int> bottomFaceIndexes;
			for (int i = 0; i < segments; i++) {
				int v1 = 2 * i + 1;
				bottomFaceIndexes.push_back(v1);
			}
			faces.push_back(
				Face(&localVertices, &globalVertices, bottomFaceIndexes)
			);

			// Side faces
			for (int i = 0; i < segments; ++i) {
				// Indices of the four vertices of the current rectangle
				int v0 = 2 * i;
				int v1 = v0 + 1;
				int v2 = (v0 + 2) % (2 * segments);
				int v3 = v2 + 1;

				// Forms faces in counter-clockwise order
				std::vector<int> sideFaceIndexes = {v0, v2, v3, v1};
				faces.push_back(
					Face(&localVertices, &globalVertices, sideFaceIndexes)
				);
			}
		}

		virtual void setEdges() override {
			for (int i = 0; i < segments; ++i) {
				/*
					Indices of the four vertices of the cylinder's
					rectangular side faces.
				*/
				int v0 = 2 * i;
				int v1 = v0 + 1;
				int v2 = (v0 + 2) % (2 * segments);
				int v3 = v2 + 1;

				/*
					We can account for all edges in the cylinder if, for each
					rectangular side face, we generate 3 edges: the top and
					bottom ones (on the bases), and one of the side edges 
					(connecting the two bases).
				*/

				edges.push_back(
					Edge(&localVertices, &globalVertices, v0, v1)
				);
				edges.push_back(
					Edge(&localVertices, &globalVertices, v1, v3)
				);
				edges.push_back(
					Edge(&localVertices, &globalVertices, v3, v2)
				);
			}
		}

	};
}


#endif