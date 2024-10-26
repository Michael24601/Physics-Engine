
#ifndef CYLINDER_H
#define CYLINDER_H

#include "mesh.h"

namespace pe {

	class Cylinder : public Mesh {

	private:

		static std::vector<Vector3D> generateCylinderVertices(
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

		static std::vector<std::vector<int>> generateFaces(int segments) {

			std::vector<std::vector<int>> faces;

			// Side faces
			for (int i = 0; i < segments; ++i) {
				// Indices of the four vertices of the current rectangle
				int v[4];
				v[0] = 2 * i;
				v[1] = (v[0] + 2) % (2 * segments);
				v[2] = v[1] + 1;
				v[3] = v[0] + 1;

				// Forms faces in counter-clockwise order
				faces.push_back(std::vector<int> { v[0], v[1], v[2], v[3] });
			}

			// Top face, in counter-clockwise order
			std::vector<int> topFaceIndexes;
			for (int i = segments - 1; i >= 0; i--) {
				int v0 = 2 * i;
				topFaceIndexes.push_back(v0);
			}
			faces.push_back(topFaceIndexes);

			// Bottom face, in counter-clockwise order
			std::vector<int> bottomFaceIndexes;
			for (int i = 0; i < segments; i++) {
				int v1 = 2 * i + 1;
				bottomFaceIndexes.push_back(v1);
			}
			faces.push_back(bottomFaceIndexes);

			return faces;
		}


		static std::vector<std::pair<int, int>> generateEdges(int segments) {

			std::vector<std::pair<int, int>> edges;

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

				edges.push_back(std::make_pair(v0, v1));
				edges.push_back(std::make_pair(v1, v3));
				edges.push_back(std::make_pair(v2, v0));
			}

			return edges;
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
			vertices[0] = getFaceVertex(0, 0);
			vertices[1] = getFaceVertex(1, 0);

			/*
				We will also need the centroid of each of those base
				faces.
			*/
			Vector3D centroids[2];
			centroids[0] = getFace(0).getCentroid();
			centroids[1] = getFace(1).getCentroid();

			/*
				And then we calculate the vector connecting each
				reference point to the centroid.
			*/
			Vector3D segments[2];
			segments[0] = vertices[0] - centroids[1];
			segments[1] = vertices[0] - centroids[1];

			// Starting with the first curved face.
			for (int i = 2; i < getFaceCount(); i++) {
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
				for (int j = 0; j < getFace(i).getVertexCount(); j++) {

					Vector3D vertex = getFaceVertex(i, j);

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

				setFaceTextureCoordinates(i, textureCoordinates);
			}
		}


		std::vector<std::vector<Vector3D>> generateVertexNormals() {

			std::vector<std::vector<Vector3D>> vertexNormals(getFaceCount());

			for (int i = 0; i < getFaceCount(); i++) {

				vertexNormals[i].resize(getFace(i).getVertexCount());

				// Flat bases
				if (i == getFaceCount() - 1 || i == getFaceCount() - 2) {
					for (int j = 0; j < getFace(i).getVertexCount(); j++) {
						vertexNormals[i][j] = getFace(i).getNormal();
					}
				}
				// For side faces, there are 4 vertices
				else {
					/*
						The normal of the side vertices is the normalized position
						vector of the vertices relative to the centre of the cylinder,
						but with a flat y-component.
					*/
					for (int j = 0; j < getFace(i).getVertexCount(); j++) {
						vertexNormals[i][j] = getFace(i).getVertex(this, j);
						vertexNormals[i][j].y = 0;
						vertexNormals[i][j].normalize();
					}
				}
			}

			return vertexNormals;
		}


	public:

		const real radius;
		const real height;
		const int segments;

		Cylinder(real radius, real height, int segments) :
			radius{ radius }, height{ height }, segments{ segments },
			Mesh(
				generateCylinderVertices(radius, height, segments), 
				generateFaces(segments), 
				generateEdges(segments)
			) {

			setVertexNormals(generateVertexNormals());
			setUVCoordinates();
		}

	};
}


#endif