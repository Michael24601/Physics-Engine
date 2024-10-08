
#ifndef CYLINDER_H
#define CYLINDER_H

#include "polyhedron.h"
#include "cuboidal.h"

namespace pe {

	class Cylinder : public Cuboidal {

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


		/*
			Sets the UV coordinates in such a way as to stretch a single
			texture over the curved area of the cylinder.
			The top and bottom faces remain with the same uv-coordinates.
		*/
		void setUVCoordinates() {
			/*
				First we find a reference point on the base faces.
			*/
			Vector3D vertices[2];
			vertices[0] = faces[0]->getVertex(0);
			vertices[1] = faces[1]->getVertex(0);

			/*
				We will also need the centroid of each of those base
				faces.
			*/
			Vector3D centroids[2];
			centroids[0] = faces[0]->getCentroid();
			centroids[1] = faces[1]->getCentroid();

			/*
				And then we calculate the vector connecting each
				reference point to the centroid.
			*/
			Vector3D segments[2];
			segments[0] = vertices[0] - centroids[1];
			segments[1] = vertices[0] - centroids[1];

			// Starting with the first curved face.
			for (int i = 2; i < faces.size(); i++) {
				std::vector<Vector2D> textureCoordinates;
				/*
					Then for each of the side faces (curved), we get each
					vertex, and depending on which base it is in, give it
					a v coordinate of 1 or 0 (up or down). Then, we can
					calculate the angle between the segment connecting
					the centroid to it, and the segment connecting the
					centroid to the reference. We then divide the result
					by 2 * pi (a full turn) to get the u coordinate.
				*/
				for (int j = 0; j < faces[i]->getVertexNumber(); j++) {

					Vector3D vertex = faces[i]->getVertex(j);

					/*
						Here we determine wether this vertex is in the top
						or bottom base face.
						We can use the magnitude squared as it is more
						efficient than the magnitude in this case.
					*/
					real distance0 = (vertex - centroids[0]).magnitudeSquared();
					real distance1 = (vertex - centroids[1]).magnitudeSquared();
					int baseIndex;
					if (distance0 < distance1) {
						baseIndex = 0;
					}
					else {
						baseIndex = 1;
					}

					/*
						Then we calculate the segment, then the angle.
						Note that there is no need to calculate the
						length of the segments, as we already know it's
						the radius.
					*/
					Vector3D segment = vertex - centroids[baseIndex];
					real angle = acos(
						segments[baseIndex].scalarProduct(segment) / (radius * radius)
					);

					real u = angle / (2 * PI);
					real v = (baseIndex == 0 ? 1 : 0);
					textureCoordinates.push_back(Vector2D(u, v));
				}

				faces[i]->setTextureCoordinates(textureCoordinates);
			}
		}


		static std::vector<Face*> generateFaces(
			std::vector<Vector3D>& localVertices,
			int segments
		) {

			std::vector<Face*> faces;

			// Top face, in counter-clockwise order
			std::vector<int> topFaceIndexes;
			for (int i = segments - 1; i >= 0; i--) {
				int v0 = 2 * i;
				topFaceIndexes.push_back(v0);
			}
			Face* topFace = new Face(
				&localVertices,
				topFaceIndexes
			);
			faces.push_back(topFace);

			// Bottom face, in counter-clockwise order
			std::vector<int> bottomFaceIndexes;
			for (int i = 0; i < segments; i++) {
				int v1 = 2 * i + 1;
				bottomFaceIndexes.push_back(v1);
			}
			Face* bottomFace = new Face(
				&localVertices,
				bottomFaceIndexes
			);
			faces.push_back(bottomFace);

			// Side faces
			for (int i = 0; i < segments; ++i) {
				// Indices of the four vertices of the current rectangle
				int v[4];
				v[0] = 2 * i;
				v[1] = (v[0] + 2) % (2 * segments);
				v[2] = v[1] + 1;
				v[3] = v[0] + 1;

				// Forms faces in counter-clockwise order
				std::vector<int> sideFaceIndexes = { v[0], v[1], v[2], v[3] };

				/*
					Note that all the side faces are curved, unlike the
					bases.
					The normals of the side face vertices can be calculated
					as the normal vector from the centroid of the base to
					which the vertices belong, to the vertices themselves.

					The normals are sent to the face in the same order as
					the vertex indexes: counter-clockwise.
				*/

				std::vector<Vector3D> normals(4);

				for (int i = 0; i < 4; i++) {
					/*
						To determine which base each vertex belongs to,
						we can calculate the vector from each centroid,
						and use the smaller (e.g. closer) one.
					*/
					Vector3D normal0 = localVertices[v[i]] -
						bottomFace->getCentroid();
					Vector3D normal1 = localVertices[v[i]] -
						topFace->getCentroid();
					if (normal0.magnitude() < normal1.magnitude()) {
						normals[i] = normal0.normalized();
					}
					else {
						normals[i] = normal1.normalized();
					}
				}

				CurvedFace* sideFace = new CurvedFace(
					&localVertices,
					sideFaceIndexes,
					normals
				);
				faces.push_back(sideFace);
			}

			return faces;
		}

		std::vector<Edge*> generateEdges(
			std::vector<Vector3D>& localVertices,
			int segments
		) {
			std::vector<Edge*> edges;

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
					(connecting the two bases). The 4th side of the rectangle
					will repeat on the next rectangle.
				*/

				edges.push_back(
					new Edge(&localVertices, v0, v1)
				);
				edges.push_back(
					new Edge(&localVertices, v1, v3)
				);
				edges.push_back(
					new Edge(&localVertices, v2, v0)
				);
			}

			return edges;
		}


	public:

		real radius;
		real length;
		int segments;

		Cylinder(
			real radius,
			real length,
			real mass,
			int segments,
			Vector3D position,
			RigidBody* body
		) :
			Cuboidal(
				mass,
				position,
				Matrix3x3(
					(1.0 / 12.0)* mass*
					(3.0 * radius * radius + length * length), 0, 0,
					0, (1.0 / 12.0)* mass *
					(3.0 * radius * radius + length * length), 0,
					0, 0, (1.0 / 12.0)* mass* (3.0 * radius * radius)
				),
				generateCylinderVertices(
					Vector3D(0, 0, 0),
					radius,
					length,
					segments
				),
				body
			),
			radius{ radius }, length{ length }, segments{ segments } {

			setFaces(generateFaces(localVertices, segments));
			setEdges(generateEdges(localVertices, segments));

			setUVCoordinates();
		}
	};
}


#endif