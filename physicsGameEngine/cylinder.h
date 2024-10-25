
#ifndef CYLINDER_H
#define CYLINDER_H

#include "rigidObject.h"
#include "orientedBoundingBox.h"

namespace pe {

	class Cylinder: public RigidObject<OrientedBoundingBox> {

	private:

		real radius;
		real height;
		int segments;

	public:

		static Mesh* generateMesh(real radius, real height, int segments) {

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

				edges.push_back(std::make_pair(v0, v1));
				edges.push_back(std::make_pair(v1, v3));
				edges.push_back(std::make_pair(v2, v0));
				edges.push_back(std::make_pair(v2, v3));
			}

			Mesh* mesh = new Mesh(vertices, faces, edges);

			// First we find a reference point on the base faces
			Vector3D referenceVertices[2];
			referenceVertices[0] = mesh->getFaceVertex(0, 0);
			referenceVertices[1] = mesh->getFaceVertex(1, 0);

			// We will also need the centroid of each of those base faces
			Vector3D centroids[2];
			centroids[0] = mesh->getFace(0).getCentroid();
			centroids[1] = mesh->getFace(1).getCentroid();

			/*
				And then we calculate the vector connecting each
				reference point to the centroid.
			*/
			Vector3D vertexSegments[2];
			vertexSegments[0] = referenceVertices[0] - centroids[1];
			vertexSegments[1] = referenceVertices[0] - centroids[1];


			for (int i = 2; i < mesh->getFaceCount(); i++) {
				std::vector<Vector2D> textureCoordinates;
				for (int j = 0; j < mesh->getFace(i).getVertexCount(); j++) {

					Vector3D vertex = mesh->getFaceVertex(i, j);

					/*
						Here we determine wether this vertex is in the top
						or bottom base face.
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

					Vector3D segment = vertex - centroids[baseIndex];
					real angle = acos(
						vertexSegments[baseIndex].scalarProduct(segment) / (radius * radius)
					);

					real u = angle / (2 * PI);
					real v = (baseIndex == 0 ? 1 : 0);
					textureCoordinates.push_back(Vector2D(u, v));
				}

				mesh->setFaceTextureCoordinates(i, textureCoordinates);
			}


			std::vector<std::vector<Vector3D>> vertexNormals(mesh->getFaceCount());

			for (int i = 0; i < mesh->getFaceCount(); i++) {

				vertexNormals[i].resize(mesh->getFace(i).getVertexCount());

				// Flat bases
				if (i == mesh->getFaceCount() - 1 || i == mesh->getFaceCount() - 2) {
					for (int j = 0; j < mesh->getFace(i).getVertexCount(); j++) {
						vertexNormals[i][j] = mesh->getFace(i).getNormal();
					}
				}
				// For side faces, there are 4 vertices
				else {
					/*
						The normal of the side vertices is the normalized position
						vector of the vertices relative to the centre of the cylinder,
						but with a flat y-component.
					*/
					for (int j = 0; j < mesh->getFace(i).getVertexCount(); j++) {
						vertexNormals[i][j] = mesh->getFace(i).getVertex(mesh, j);
						vertexNormals[i][j].y = 0;
						vertexNormals[i][j].normalize();
					}
				}
			}

			mesh->setVertexNormals(vertexNormals);

			return mesh;
		}


		Cylinder(
			real radius, real height, int segments,
			const Vector3D& position,
			const Quaternion& orientation,
			real mass,
			bool smooth
		) : RigidObject<OrientedBoundingBox>(
			generateMesh(radius, height, segments),
			new Renderer(mesh, GL_STATIC_DRAW, smooth),
			new OrientedBoundingBox(Vector3D(radius, 0, radius)),
			position,
			orientation,
			mass,
			Matrix3x3(
				(1.0 / 12.0)* mass*
				(3.0 * radius * radius + height * height), 0, 0,
				0, (1.0 / 12.0)* mass*
				(3.0 * radius * radius + height * height), 0,
				0, 0, (1.0 / 12.0)* mass* (3.0 * radius * radius)
			)
		), radius{ radius }, height{ height }, segments{ segments } {}


		~Cylinder() {
			delete mesh;
			delete renderer;
			delete boundingVolume;
		}

	};
}


#endif