
#ifndef SPHERE_H
#define SPHERE_H

#include "mesh.h"

namespace pe {

	class Sphere : public Mesh {

	public:

		// Tesselates sphere by generating vertices
		static std::vector<Vector3D> generateVertices(
			real radius, 
			int latitudeSegments,
			int longitudeSegments
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
			return vertices;
		}


		static std::vector<std::vector<int>> generateFaces(
			int latitudeSegments,
			int longitudeSegments
		) {

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

			return faces;
		}


		static std::vector<std::pair<int, int>> generateEdges(
			int latitudeSegments,
			int longitudeSegments
		) {

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

			return edges;
		}


		std::vector<std::vector<Vector3D>> generateVertexNormals() {

			std::vector<std::vector<Vector3D>> vertexNormals(getFaceCount());

			for (int i = 0; i < getFaceCount(); i++) {

				vertexNormals[i].resize(getFace(i).getVertexCount());

				/*
					For a sphere, the vertex normals of each point is just the
					normalized position vector relative to the centre.
				*/
				for (int j = 0; j < getFace(i).getVertexCount(); j++) {
					vertexNormals[i][j] = getFaceVertex(i, j);
					vertexNormals[i][j].normalize();
				}
			}

			return vertexNormals;
		}


		/*
			Sets the UV coordinates in such a way as to stretch a single
			texture over the whole sphere.
		*/
		void setUVCoordinates() {
			for (int i = 0; i < getFaceCount(); i++) {

				std::vector<Vector2D> textureCoordinates;
				for (int j = 0; j < getFace(i).getVertexCount(); j++) {

					int index = getFace(i).getIndex(j);

					/*
						The vector from the centre of the sphere to the
						vertex, with the sphere's poles along the y-axis
						(initially, in local coordinates).
						We then normalize d or divide d.y by the radius
						in the phi calculation.
					*/
					Vector3D d = getVertex(index);
					d.normalize();

					real theta = std::atan2(d.z, d.x);
					real phi = std::asin(d.y);

					real u = -0.5 - theta / (2 * PI);
					real v = 0.5 + phi / PI;
					textureCoordinates.push_back(Vector2D(u, v));
				}

				setFaceTextureCoordinates(i, textureCoordinates);
			}
		}


	public:

		const real radius;
		const int latitudeSegments;
		const int longitudeSegments;

		Sphere(real radius, int latitudeSegments, int longitudeSegments) :
			radius{ radius }, latitudeSegments{ latitudeSegments },
			longitudeSegments{ longitudeSegments },
			Mesh(
				generateVertices(radius, latitudeSegments, longitudeSegments), 
				generateFaces(latitudeSegments, longitudeSegments),
				generateEdges(latitudeSegments, longitudeSegments)
			) {

			/*
				Because a sphere is a rigid object that can't be deformed,
				and because it has a transform matrix, there is no need
				to generate a curvature map, and we can instead directly set
				and later update the vertex normals.
			*/
			setVertexNormals(generateVertexNormals());
			setUVCoordinates();
		}

	};
}


#endif

