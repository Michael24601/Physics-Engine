
#ifndef SOLID_SPHERE_H
#define SOLID_SPHERE_H

#include "polyhedron.h"

namespace pe {

	class SolidSphere : public Polyhedron {

	private:

		// Tesselates sphere by generating vertices
		static std::vector<Vector3D> generateSphereVertices(
			const Vector3D& center,
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
					v += center;

					vertices.push_back(v);
				}
			}
			return vertices;
		}


		/*
			Sets the UV coordinates in such a way as to stretch a single
			texture over the whole sphere.
		*/
		void setUVCoordinates() {
			for (int i = 0; i < faces.size(); i++) {

				std::vector<Vector2D> textureCoordinates;
				for (int j = 0; j < faces[i]->getVertexNumber(); j++) {

					int index = faces[i]->getIndex(j);
					/*
						The vector from the centre of the sphere to the
						vertex, with the sphere's poles along the y-axis
						(initially, in local coordinates).
						We then normalize d or divide d.y by the radius
						in the phi calculation.

					*/
					Vector3D d = localVertices[index];
					d.normalize();

					real theta = std::atan2(d.z, d.x);
					real phi = std::asin(d.y);

					real u = -0.5 - theta / (2 * PI);
					real v = 0.5 + phi / PI;
					textureCoordinates.push_back(Vector2D(u, v));
				}

				faces[i]->setTextureCoordinates(textureCoordinates);
			}
		}


	public:

		real radius;
		int latitudeSegments;
		int longitudeSegments;

		SolidSphere(
			real radius,
			real mass,
			int latitudeSegments,
			int longtitudeSegments,
			Vector3D position,
			RigidBody* body
		) :
			Polyhedron(
				mass,
				position,
				Matrix3x3(
					(2.0 / 5.0)* mass* radius* radius, 0, 0,
					0, (2.0 / 5.0)* mass* radius* radius, 0,
					0, 0, (2.0 / 5.0)* mass* radius* radius
				),
				generateSphereVertices(
					Vector3D(0, 0, 0),
					radius,
					latitudeSegments,
					longtitudeSegments
				),
				body
			),
			radius{ radius }, latitudeSegments{ latitudeSegments },
			longitudeSegments{ longtitudeSegments } {

			setEdges();
			setFaces();

			setUVCoordinates();
		}


		virtual void setFaces() override {
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
					std::vector<int> faceIndexes{ v[0], v[1], v[3], v[2]};

					/*
						Note that all the sphere faces are curved.
						The normals of the side face vertices can be calculated
						as the normal vector from the center of the sphere to
						which the vertices themselves.
						Note that since the normals are initially sent in local
						vertices, we can just consider the center to be the 
						origin (0, 0, 0) and the normal of each vertex the
						vertex coordinates themselves normalized.

						The normals are sent to the face in the same order as
						the vertex indexes: counter-clockwise.
					*/

					std::vector<Vector3D> normals{
						localVertices[v[0]].normalized(),
						localVertices[v[1]].normalized(),
						localVertices[v[3]].normalized(),
						localVertices[v[2]].normalized()
					};

					CurvedFace* face = new CurvedFace(
						&localVertices,
						&globalVertices,
						faceIndexes,
						normals
					);
					faces.push_back(face);
				}
			}
		}


		virtual void setEdges() override {
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

					edges.push_back(
						new Edge(&localVertices, &globalVertices, v0, v2)
					);
					edges.push_back(
						new Edge(&localVertices, &globalVertices, v3, v1)
					);
					edges.push_back(
						new Edge(&localVertices, &globalVertices, v2, v3)
					);
					edges.push_back(
						new Edge(&localVertices, &globalVertices, v1, v0)
					);
				}
			}
		}

	};
}


#endif