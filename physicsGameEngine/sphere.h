
#ifndef SPHERE_H
#define SPHERE_H

#include "rigidObject.h"

namespace pe {

	class Sphere : public RigidObject<BoundingSphere> {

	private:

		real radius;
		int latitudeSegments;
		int longitudeSegments;

	public:

		// Tesselates sphere by generating vertices
		static Mesh* generateMesh(
			real radius, int latitudeSegments, int longitudeSegments
		) {

			std::vector<Vector3D> vertices;
			for (int lat = 0; lat <= latitudeSegments; ++lat) {
				for (int lon = 0; lon <= longitudeSegments; ++lon) {
					// Vertices of the current square
					Vector3D v;

					real phi = 2.0f * PI * static_cast<real>(lon)
						/ static_cast<real>(longitudeSegments);
					real theta = PI * static_cast<real>(lat)
						/ static_cast<real>(latitudeSegments);

					// Calculate the vertex
					v += Vector3D(
						sin(theta) * cos(phi),
						cos(theta),
						sin(theta) * sin(phi)
					);
					v.normalize();
					v *= radius;

					vertices.push_back(v);
				}
			}

			std::vector<std::vector<int>> faces;
			// Generates sphere faces based on how the vertices were
			for (int lat = 0; lat < latitudeSegments; lat++) {

				for (int lon = 0; lon < longitudeSegments; lon++) {
					/*
						Indices of the four vertices of the current triangle
						in counterclockwise order.
					*/
					int v[4];
					v[0] = lat * (longitudeSegments + 1) + lon;
					v[1] = v[0] + 1;
					v[2] = (lat + 1) * (longitudeSegments + 1) + lon;
					v[3] = v[2] + 1;

					// Forms face in counter-clockwise order
					faces.push_back(std::vector<int>{ v[0], v[1], v[3], v[2] });
				}
			}

			std::vector<std::pair<int, int>> edges;

			// Generates sphere edges based on how the vertices were
			for (int lat = 0; lat < latitudeSegments; lat++) {
				for (int lon = 0; lon < longitudeSegments; lon++) {
					/*
						Indices of the four vertices of the current square
						in counter clockwise order
					*/
					int v0 = lat * (longitudeSegments + 1) + lon;
					int v1 = v0 + 1;
					int v2 = (lat + 1) * (longitudeSegments + 1) + lon;
					int v3 = v2 + 1;

					edges.push_back(std::make_pair(v0, v2));
					edges.push_back(std::make_pair(v3, v1));
					edges.push_back(std::make_pair(v2, v3));
					edges.push_back(std::make_pair(v1, v0));

				}
			}

			Mesh* mesh = new Mesh(vertices, faces, edges);

			// Vertex normals for curvature
			std::vector<std::vector<Vector3D>> vertexNormals(mesh->getFaceCount());
			for (int i = 0; i < mesh->getFaceCount(); i++) {

				vertexNormals[i].resize(mesh->getFace(i).getVertexCount());

				/*
					For a sphere, the vertex normals of each point is just the
					normalized position vector relative to the centre.
				*/
				for (int j = 0; j < mesh->getFace(i).getVertexCount(); j++) {
					vertexNormals[i][j] = mesh->getFaceVertex(i, j);
					vertexNormals[i][j].normalize();
				}
			}

			mesh->setVertexNormals(vertexNormals);

			// UV coordinates
			for (int i = 0; i < mesh->getFaceCount(); i++) {

				std::vector<Vector2D> textureCoordinates;
				for (int j = 0; j < mesh->getFace(i).getVertexCount(); j++) {

					int index = mesh->getFace(i).getIndex(j);

					/*
						The vector from the centre of the sphere to the
						vertex, with the sphere's poles along the y-axis
						(initially, in local coordinates).
						We then normalize d or divide d.y by the radius
						in the phi calculation.
					*/
					Vector3D d = mesh->getVertex(index);
					d.normalize();

					real theta = std::atan2(d.z, d.x);
					real phi = std::asin(d.y);

					real u = -0.5 - theta / (2 * PI);
					real v = 0.5 + phi / PI;
					textureCoordinates.push_back(Vector2D(u, v));
				}

				mesh->setFaceTextureCoordinates(i, textureCoordinates);
			}

			return mesh;
		}


		Sphere(
			real radius, int latitudeSegments, int longitudeSegments,
			const Vector3D& position,
			const Quaternion& orientation,
			real mass,
			bool smooth
		) : RigidObject<BoundingSphere>(
			generateMesh(radius, latitudeSegments, longitudeSegments),
			new Renderer(mesh, GL_STATIC_DRAW, smooth),
			new BoundingSphere(radius),
			position,
			orientation,
			mass,
			Matrix3x3(
				(2.0 / 5.0)* mass* radius* radius, 0, 0,
				0, (2.0 / 5.0)* mass* radius* radius, 0,
				0, 0, (2.0 / 5.0)* mass* radius* radius
			)
		), radius{ radius }, latitudeSegments{ latitudeSegments }, 
			longitudeSegments{ longitudeSegments }{}


		~Sphere() {
			delete mesh;
			delete renderer;
			delete boundingVolume;
		}

	};
}


#endif