
#ifndef CONE_H
#define CONE_H

#include "mesh.h"

namespace pe {

	class Cone : public Mesh {

	private:

		/*
			Defines the vertices of a tessalated cone with a certain radius,
			height, and a number of segments (the more segments the more
			it will look like a cone).
		*/
		static std::vector<Vector3D> generateVertices(
			real radius, 
			real height, 
			int segments
		) {

			std::vector<Vector3D> vertices;

			// Adds the apex of the cone at the top
			vertices.push_back(Vector3D(0.0f, 3 * height / 4.0f, 0.0f));

			for (int i = 0; i <= segments; ++i) {
				// Calculate the angle for each segment
				real theta = 2.0f * PI * static_cast<real>(i) /
					static_cast<real>(segments);

				// Vertices of the current point on the base of the cone
				Vector3D v_base = Vector3D(
					radius * cos(theta),
					-height / 4.0f,
					radius * sin(theta)
				);

				vertices.push_back(v_base);
			}

			return vertices;
		}


		/*
			Assumes the vertices and segment length have been set.
		*/
		static std::vector<std::vector<int>> generateFaces(int segments) {

			std::vector<std::vector<int>> faces;

			// Side faces
			for (int i = 1; i <= segments; ++i) {
				int v0 = i;
				int v1 = (i % segments) + 1;

				faces.push_back(std::vector<int>{ 0, v1, v0 });
			}

			// Base face
			std::vector<int> baseFaceIndexes;
			for (int i = 1; i <= segments; i++) {
				baseFaceIndexes.push_back(i);
			}
			faces.push_back(baseFaceIndexes);

			return faces;
		}


		static std::vector<std::pair<int, int>> generateEdges(int segments) {

			std::vector<std::pair<int, int>> edges;

			// Base edges
			for (int i = 1; i <= segments; ++i) {
				// Indexes of the two vertices of the current edge
				int v0 = i;
				int v1 = (i % segments) + 1;

				// Connects vertices to form edges based on the side faces
				edges.push_back(std::make_pair(v0, v1));
			}

			// Edges from the apex to the base vertices
			for (int i = 1; i <= segments; ++i) {
				edges.push_back(std::make_pair(0, i));
			}

			return edges;
		}


		std::vector<std::vector<Vector3D>> generateVertexNormals() {

			std::vector<std::vector<Vector3D>> vertexNormals(getFaceCount());

			for (int i = 0; i < getFaceCount(); i++) {

				vertexNormals[i].resize(getFace(i).getVertexCount());

				// Flat base
				if (i == getFaceCount() - 1) {
					for (int j = 0; j < getFace(i).getVertexCount(); j++) {
						vertexNormals[i][j] = getFace(i).getNormal();
					}
				}
				// For side faces, there are 3 vertices
				else {
					// Apex, has the reverse of the base
					vertexNormals[i][0] = getFace(getFaceCount() - 1).getNormal().inverse();
					/*
						The normal of the side vertices is the normalized position
						vector of the vertices relative to the centre of the cone,
						but with a flat y-component.
					*/
					vertexNormals[i][1] = getFace(i).getVertex(this, 1);
					vertexNormals[i][1].y = 0;
					vertexNormals[i][1].normalize();
					vertexNormals[i][2] = getFace(i).getVertex(this, 2);
					vertexNormals[i][2].y = 0;
					vertexNormals[i][2].normalize();
				}
			}

			return vertexNormals;
		}


	public:

		const real radius;
		const real height;
		const int segments;

		Cone(real radius, real heigth, int segments) :
			radius{ radius }, height{ height }, segments{ segments },
			Mesh(
				generateVertices(radius, height, segments), 
				generateFaces(segments), 
				generateEdges(segments)
			){

			setVertexNormals(generateVertexNormals());
		}

	};
}


#endif

