
#ifndef CONE_H
#define CONE_H

#include "rigidObject.h"
#include "orientedBoundingBox.h"

namespace pe {

	class Cone : public RigidObject<OrientedBoundingBox> {

	private:

		real radius;
		real height;
		int segments;

	public:

		static Mesh* generateMesh(real radius, real height, int segments) {

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

			Mesh* mesh = new Mesh(vertices, faces, edges);
			
			// Vertex normals
			std::vector<std::vector<Vector3D>> vertexNormals(mesh->getFaceCount());

			for (int i = 0; i < mesh->getFaceCount(); i++) {

				vertexNormals[i].resize(mesh->getFace(i).getVertexCount());

				// Flat base
				if (i == mesh->getFaceCount() - 1) {
					for (int j = 0; j < mesh->getFace(i).getVertexCount(); j++) {
						vertexNormals[i][j] = mesh->getFace(i).getNormal();
					}
				}
				// For side faces, there are 3 vertices
				else {
					// Apex, has the reverse of the base
					vertexNormals[i][0] = 
						mesh->getFace(mesh->getFaceCount() - 1).getNormal().inverse();
					/*
						The normal of the side vertices is the normalized position
						vector of the vertices relative to the centre of the cone,
						but with a flat y-component.
					*/
					vertexNormals[i][1] = mesh->getFace(i).getVertex(mesh, 1);
					vertexNormals[i][1].y = 0;
					vertexNormals[i][1].normalize();
					vertexNormals[i][2] = mesh->getFace(i).getVertex(mesh, 2);
					vertexNormals[i][2].y = 0;
					vertexNormals[i][2].normalize();
				}
			}

			mesh->setVertexNormals(vertexNormals);
			
			return mesh;
		}


		Cone(
			real radius, real height, int segments,
			const Vector3D& position,
			const Quaternion& orientation,
			real mass,
			bool smooth
		) : RigidObject<OrientedBoundingBox>(
			generateMesh(radius, height, segments),
			new Renderer(mesh, GL_STATIC_DRAW, smooth),
			/*
				The centroid of the cone is one fourth of the way
				up from the base. The bounding box is centred halfway
				between the base and apex of the pyramid, so we have
				to shift it by a quarter of the height.
				The halfsize of the bounding box is the radius,
				half the height, then the radius.
			*/
			new OrientedBoundingBox(
				Vector3D(radius, height / 2.0, radius),
				Vector3D(0, height / 4.0, 0)
			),
			position,
			orientation,
			mass,
			Matrix3x3(
				(3.0 / 80.0)* mass* (radius* radius + 4.0 * height * height), 0, 0,
				0, (3.0 / 80.0)* mass* (radius* radius + 4.0 * height * height), 0,
				0, 0, (3.0 / 40.0)* mass* radius* radius
			)
		), radius{ radius }, height{ height }, segments{segments} {}


		~Cone() {
			delete mesh;
			delete renderer;
			delete boundingVolume;
		}

	};
}


#endif